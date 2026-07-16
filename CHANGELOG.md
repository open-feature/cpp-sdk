# Changelog

## [0.1.3](https://github.com/open-feature/cpp-sdk/compare/v0.1.2...v0.1.3) (2026-07-16)


### Bug Fixes

* race condition between status check and provider lookup ([#108](https://github.com/open-feature/cpp-sdk/issues/108)) ([59b2342](https://github.com/open-feature/cpp-sdk/commit/59b234239bfb5af3c92e3101b24da71e0a38969f))

## [0.1.2](https://github.com/open-feature/cpp-sdk/compare/v0.1.1...v0.1.2) (2026-07-14)


### Features

* Add HookContext class ([#115](https://github.com/open-feature/cpp-sdk/issues/115)) ([ec5a971](https://github.com/open-feature/cpp-sdk/commit/ec5a97101d886fc256803353808957ac9b5885b7))
* Add HookData class.  ([#114](https://github.com/open-feature/cpp-sdk/issues/114)) ([fdac1e0](https://github.com/open-feature/cpp-sdk/commit/fdac1e00715884662c5a1a96054625c2696b9f55))
* Base data structures fot hook implementation. ([#112](https://github.com/open-feature/cpp-sdk/issues/112)) ([91d1b13](https://github.com/open-feature/cpp-sdk/commit/91d1b13a2cc74bc456dd31d3483be19ad619ac4f))

## [0.1.1](https://github.com/open-feature/cpp-sdk/compare/v0.1.0...v0.1.1) (2026-06-23)


### Bug Fixes

* Correct logic for evaluation blocked for ERROR and STALE providers ([#96](https://github.com/open-feature/cpp-sdk/issues/96)) ([9610f71](https://github.com/open-feature/cpp-sdk/commit/9610f71570119dae5e888dbdc7a5cc64a6eb5291))

## [0.1.0](https://github.com/open-feature/cpp-sdk/compare/v0.0.2...v0.1.0) (2026-06-23)


### ⚠ BREAKING CHANGES

* provider exceptions crash the caller ([#97](https://github.com/open-feature/cpp-sdk/issues/97))

### Bug Fixes

* provider exceptions crash the caller ([#97](https://github.com/open-feature/cpp-sdk/issues/97)) ([f4d3423](https://github.com/open-feature/cpp-sdk/commit/f4d3423ab656291030ddf0349d4e158533cc0e79))

## [0.0.2](https://github.com/open-feature/cpp-sdk/compare/v0.0.1...v0.0.2) (2026-06-23)


### Features

* add release-please configuration and manifest with initial version 0.0.1 ([#85](https://github.com/open-feature/cpp-sdk/issues/85)) ([25425bf](https://github.com/open-feature/cpp-sdk/commit/25425bf2100476123c3df2cb3e8caf55bfb7d575))


### Bug Fixes

* old provider not shutting down when the new provider fails to initialize ([#98](https://github.com/open-feature/cpp-sdk/issues/98)) ([aa27439](https://github.com/open-feature/cpp-sdk/commit/aa27439e84b60b94deb33f7b736f2d652631f90f))
* remove $schema field from .release-please-manifest.json ([#87](https://github.com/open-feature/cpp-sdk/issues/87)) ([28c283c](https://github.com/open-feature/cpp-sdk/commit/28c283c428993ec46775aa8d275765a24f340ee0))
