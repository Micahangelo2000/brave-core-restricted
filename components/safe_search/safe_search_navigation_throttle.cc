// safe_search_navigation_throttle.cc
#include "components/safe_search/safe_search_navigation_throttle.h"

#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/open_url_params.h"
#include "content/public/browser/navigation_controller.h"
#include "url/gurl.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"

namespace safe_search {

SafeSearchNavigationThrottle::SafeSearchNavigationThrottle(
    content::NavigationHandle* handle)
    : content::NavigationThrottle(handle) {}

SafeSearchNavigationThrottle::~SafeSearchNavigationThrottle() = default;

std::unique_ptr<content::NavigationThrottle>
SafeSearchNavigationThrottle::MaybeCreateThrottleFor(
    content::NavigationHandle* handle) {
  // Always create — do NOT gate on IsOffTheRecord() per your design.
  return std::make_unique<SafeSearchNavigationThrottle>(handle);
}

bool SafeSearchNavigationThrottle::HostIsSearchProvider(const GURL& url) const {
  std::string host = url.host();
  // Basic host checks. Expand this list (regional TLDs, images., encrypted., etc.)
  if (host == "www.google.com" || host.find("google.") == 0) return true;
  if (host == "www.bing.com" || host == "bing.com") return true;
  if (host == "duckduckgo.com" || host == "www.duckduckgo.com" ||
      host == "safe.duckduckgo.com")
    return true;
  return false;
}

GURL SafeSearchNavigationThrottle::MakeSafeUrl(const GURL& url) const {
  std::string host = url.host();
  GURL::Replacements repl;
  if (host.find("google.") != std::string::npos) {
    // append/replace safe=active
    GURL::Replacements rep;
    rep.SetQueryStr((url.has_query() ? url.query() + "&safe=active" : "safe=active"));
    return url.ReplaceComponents(rep);
  }
  if (host.find("duckduckgo.com") != std::string::npos) {
    GURL::Replacements rep;
    rep.SetQueryStr((url.has_query() ? url.query() + "&kp=1" : "kp=1"));
    return url.ReplaceComponents(rep);
  }
  if (host.find("bing.com") != std::string::npos) {
    GURL::Replacements rep;
    rep.SetQueryStr((url.has_query() ? url.query() + "&safeSearch=Strict" : "safeSearch=Strict"));
    return url.ReplaceComponents(rep);
  }
  return url;
}

content::NavigationThrottle::ThrottleCheckResult
SafeSearchNavigationThrottle::WillStartRequest() {
  GURL url = navigation_handle()->GetURL();
  if (!HostIsSearchProvider(url)) return PROCEED;
  GURL safe = MakeSafeUrl(url);
  if (safe == url) return PROCEED;

  // Redirect: cancel original and open safe URL in same tab
  content::WebContents* wc = navigation_handle()->GetWebContents();
  content::OpenURLParams params(safe, content::Referrer(), WindowOpenDisposition::CURRENT_TAB,
                               ui::PAGE_TRANSITION_TYPED, false);
  wc->OpenURL(params);
  return CANCEL_AND_IGNORE;
}

content::NavigationThrottle::ThrottleCheckResult
SafeSearchNavigationThrottle::WillRedirectRequest() {
  // If server redirect leads to a non-safe host, re-evaluate and reapply.
  return WillStartRequest();
}

}  // namespace safe_search
