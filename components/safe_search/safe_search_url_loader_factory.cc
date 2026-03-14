// safe_search_url_loader_factory.h - conceptual
class SafeSearchURLLoaderFactory : public network::mojom::URLLoaderFactory {
 public:
  SafeSearchURLLoaderFactory(mojo::PendingReceiver<network::mojom::URLLoaderFactory> receiver,
                             network::mojom::URLLoaderFactory* inner_factory);
  ~SafeSearchURLLoaderFactory() override;

  // network::mojom::URLLoaderFactory:
  void CreateLoaderAndStart(...) override {
    // Inspect resource_request->url
    // If host is a blocked-search engine or a search provider without safe param:
    //   - modify url (append param) OR
    //   - return a synthetic redirect response to the safe host (preferable)
    // Otherwise delegate to inner_factory_->CreateLoaderAndStart(...)
  }
  void Clone(mojo::PendingReceiver<network::mojom::URLLoaderFactory> receiver) override;
 private:
  network::mojom::URLLoaderFactory* inner_factory_;
};
