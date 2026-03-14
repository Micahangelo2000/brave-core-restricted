// safe_search_navigation_throttle.h
#ifndef BRAVE_SAFE_SEARCH_SAFE_SEARCH_NAVIGATION_THROTTLE_H_
#define BRAVE_SAFE_SEARCH_SAFE_SEARCH_NAVIGATION_THROTTLE_H_

#include "content/public/browser/navigation_throttle.h"
#include "url/gurl.h"

namespace safe_search {

class SafeSearchNavigationThrottle : public content::NavigationThrottle {
 public:
  explicit SafeSearchNavigationThrottle(content::NavigationHandle* handle);
  ~SafeSearchNavigationThrottle() override;

  // NavigationThrottle:
  ThrottleCheckResult WillStartRequest() override;
  ThrottleCheckResult WillRedirectRequest() override;

  static std::unique_ptr<content::NavigationThrottle> MaybeCreateThrottleFor(
      content::NavigationHandle* handle);

 private:
  bool HostIsSearchProvider(const GURL& url) const;
  GURL MakeSafeUrl(const GURL& url) const;
};

}  // namespace safe_search

#endif  // BRAVE_SAFE_SEARCH_SAFE_SEARCH_NAVIGATION_THROTTLE_H_
