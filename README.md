
<!-- markdownlint-disable MD033 -->
<!-- x-hide-in-docs-start -->
<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/open-feature/community/0e23508c163a6a1ac8c0ced3e4bd78faafe627c7/assets/logo/horizontal/white/openfeature-horizontal-white.svg" />
    <img align="center" alt="OpenFeature Logo" src="https://raw.githubusercontent.com/open-feature/community/0e23508c163a6a1ac8c0ced3e4bd78faafe627c7/assets/logo/horizontal/black/openfeature-horizontal-black.svg" />
  </picture>
</p>

<h2 align="center">OpenFeature C++ SDK</h2>

<!-- x-hide-in-docs-end -->
<!-- The 'github-badges' class is used in the docs -->
<p align="center" class="github-badges">

  <a href="https://github.com/open-feature/spec/releases/tag/v0.2.0">
    <img alt="Specification" src="https://img.shields.io/static/v1?label=specification&message=v0.2.0&color=yellow&style=for-the-badge" />
  </a>
  <a href="https://github.com/open-feature/cpp-sdk/releases/tag/v0.0.1">
    <img alt="Release" src="https://img.shields.io/static/v1?label=release&message=v0.0.1&color=blue&style=for-the-badge" />
  </a>  
  <!-- x-release-please-end -->
  <br/>
  <a href="https://bestpractices.coreinfrastructure.org/projects/6601">
    <img alt="CII Best Practices" src="https://bestpractices.coreinfrastructure.org/projects/6601/badge" />
  </a>
</p>
<!-- x-hide-in-docs-start -->

[OpenFeature](https://openfeature.dev) is an open specification that provides a vendor-agnostic, community-driven API for feature flagging that works with your favorite feature flag management tool or in-house solution.

<!-- x-hide-in-docs-end -->
## 🚀 Quick start

### Requirements

<!-- TODO: required runtime, etc -->

### Install

<!-- TODO: installation instructions -->

### Usage

<!-- TODO: basic usage instructions, setting the in-memory provider and getting a boolean flag called "v2_enabled" -->

### API Reference

<!-- TODO: link to formal API docs (ie: Javadoc) if available -->

## 🌟 Features

<!-- TODO: update table to indicate implemented features (see legend below) -->

| Status | Features                                                            | Description                                                                                                                                                  |
| ------ | ------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| ❌      | [Providers](#providers)                                             | Integrate with a commercial, open source, or in-house feature management tool.                                                                               |
| ❌      | [Targeting](#targeting)                                             | Contextually-aware flag evaluation using [evaluation context](https://openfeature.dev/docs/reference/concepts/evaluation-context).                           |
| ❌      | [Hooks](#hooks)                                                     | Add functionality to various stages of the flag evaluation life-cycle.                                                                                       |
| ❌      | [Logging](#logging)                                                 | Integrate with popular logging packages.                                                                                                                     |
| ❌      | [Domains](#domains)                                                 | Logically bind clients with providers.                                                                                                                       |
| ❌      | [Eventing](#eventing)                                               | React to state changes in the provider or flag management system.                                                                                            |
| ❌      | [Tracking](#tracking)                                               | Associate user-actions with flag evaluations for the purposes of experimentation.                                                                            |
| ❌      | [Transaction Context Propagation](#transaction-context-propagation) | Set a specific [evaluation context](https://openfeature.dev/docs/reference/concepts/evaluation-context) for a transaction (e.g. an HTTP request or a thread) |
| ❌      | [Shutdown](#shutdown)                                               | Gracefully clean up a provider during application shutdown.                                                                                                  |
| ❌      | [Extending](#extending)                                             | Extend OpenFeature with custom providers and hooks.                                                                                                          |

<sub>Implemented: ✅ | In-progress: ⚠️ | Not implemented yet: ❌</sub>

### Providers

[Providers](https://openfeature.dev/docs/reference/concepts/provider) are an abstraction between a flag management system and the OpenFeature SDK.
Look [here](https://openfeature.dev/ecosystem?instant_search%5BrefinementList%5D%5Btype%5D%5B0%5D=Provider&instant_search%5BrefinementList%5D%5Btechnology%5D%5B0%5D=cpp) for a complete list of available providers.
If the provider you're looking for hasn't been created yet, see the [develop a provider](#develop-a-provider) section to learn how to build it yourself.

Once you've added a provider as a dependency, it can be registered with OpenFeature like this:

<!-- TODO: code example setting a provider and setting it while awaiting init, if applicable -->

In some situations, it may be beneficial to register multiple providers in the same application.
This is possible using [domains](#domains), which is covered in more detail below.

### Targeting

Sometimes, the value of a flag must consider some dynamic criteria about the application or user, such as the user's location, IP, email address, or the server's location.
In OpenFeature, we refer to this as [targeting](https://openfeature.dev/specification/glossary#targeting).
If the flag management system you're using supports targeting, you can provide the input data using the [evaluation context](https://openfeature.dev/docs/reference/concepts/evaluation-context).

<!-- TODO: code examples using context and different levels -->


### Hooks

[Hooks](https://openfeature.dev/docs/reference/concepts/hooks) allow for custom logic to be added at well-defined points of the flag evaluation life-cycle.
Look [here](https://openfeature.dev/ecosystem/?instant_search%5BrefinementList%5D%5Btype%5D%5B0%5D=Hook&instant_search%5BrefinementList%5D%5Btechnology%5D%5B0%5D=cpp) for a complete list of available hooks.
If the hook you're looking for hasn't been created yet, see the [develop a hook](#develop-a-hook) section to learn how to build it yourself.

Once you've added a hook as a dependency, it can be registered at the global, client, or flag invocation level.

<!-- TODO: code example of setting hooks at all levels -->

### Logging

<!-- TODO: talk about logging config and include a code example -->

### Domains

Clients can be assigned to a domain. A domain is a logical identifier which can be used to associate clients with a particular provider. 
If a domain has no associated provider, the default provider is used.

<!-- TODO: code example binding a named client to a provider -->

### Eventing

Events allow you to react to state changes in the provider or underlying flag management system, such as flag definition changes, provider readiness, or error conditions.
Initialization events (`PROVIDER_READY` on success, `PROVIDER_ERROR` on failure) are dispatched for every provider.
Some providers support additional events, such as `PROVIDER_CONFIGURATION_CHANGED`.

Please refer to the documentation of the provider you're using to see what events are supported.

<!-- TODO: code example of a PROVIDER_CONFIGURATION_CHANGED event for the client and a PROVIDER_STALE event for the API -->

### Tracking

The tracking API allows you to use OpenFeature abstractions and objects to associate user actions with feature flag evaluations.
This is essential for robust experimentation powered by feature flags.
For example, a flag enhancing the appearance of a UI component might drive user engagement to a new feature; to test this hypothesis, telemetry collected by a [hook](#hooks) or [provider](#providers) can be associated with telemetry reported in the client's `track` function.

<!-- TODO: code example for tracking -->

Note that some providers may not support tracking; check the documentation for your provider for more information.

### Transaction Context Propagation

Transaction context is a container for transaction-specific evaluation context (e.g. user id, user agent, IP).
Transaction context can be set where specific data is available (e.g. an auth service or request handler) and by using the transaction context propagator it will automatically be applied to all flag evaluations within a transaction (e.g. a request or thread).

<!-- TODO: code example for transaction context propagation -->

### Shutdown

The OpenFeature API provides a close function to perform a cleanup of all registered providers.
This should only be called when your application is in the process of shutting down.

<!-- TODO: code example for global shutdown -->

## Extending

### Develop a provider

To develop a provider, you need to create a new project and include the OpenFeature SDK as a dependency.
This can be a new repository or included in [the existing contrib repository](https://github.com/open-feature/cpp-sdk-contrib) available under the OpenFeature organization.
You’ll then need to write the provider by implementing the `FeatureProvider` interface exported by the OpenFeature SDK.

<!-- TODO: code example of provider implementation -->

> Built a new provider? [Let us know](https://github.com/open-feature/openfeature.dev/issues/new?assignees=&labels=provider&projects=&template=document-provider.yaml&title=%5BProvider%5D%3A+) so we can add it to the docs!

### Develop a hook

To develop a hook, you need to create a new project and include the OpenFeature SDK as a dependency.
This can be a new repository or included in [the existing contrib repository](https://github.com/open-feature/cpp-sdk-contrib) available under the OpenFeature organization.
Implement your own hook by conforming to the `Hook interface`.
To satisfy the interface, all methods (`Before`/`After`/`Finally`/`Error`) need to be defined.
To avoid defining empty functions, make use of the `UnimplementedHook` struct (which already implements all the empty functions).

<!-- TODO: code example of hook implementation -->

> Built a new hook? [Let us know](https://github.com/open-feature/openfeature.dev/issues/new?assignees=&labels=hook&projects=&template=document-hook.yaml&title=%5BHook%5D%3A+) so we can add it to the docs!

<!-- x-hide-in-docs-start -->
## ⭐️ Support the project

- Give this repo a ⭐️!
- Follow us on social media:
  - Twitter: [@openfeature](https://twitter.com/openfeature)
  - LinkedIn: [OpenFeature](https://www.linkedin.com/company/openfeature/)
- Join us on [Slack](https://cloud-native.slack.com/archives/C0344AANLA1)
- For more, check out our [community page](https://openfeature.dev/community/)

## 🤝 Contributing

Interested in contributing? Great, we'd love your help! To get started, take a look at the [CONTRIBUTING](CONTRIBUTING.md) guide.

### Thanks to everyone who has already contributed

<a href="https://github.com/open-feature/cpp-sdk/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=open-feature/cpp-sdk" alt="Pictures of the folks who have contributed to the project"/>
</a>


Made with [contrib.rocks](https://contrib.rocks).
<!-- x-hide-in-docs-end -->
