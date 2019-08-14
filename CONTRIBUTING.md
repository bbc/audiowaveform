# Contributing

Thank you for your interest in audiowaveform!

We love hearing feedback from people who use our software, so if you are using this for something interesting, please let us know.

Contributions are welcomed and encouraged. If you're thinking of writing a new feature, please first discuss the change you wish to make, either by raising an issue, or contacting us directly, e.g, [by email](mailto:irfs@bbc.co.uk).

## Making changes

* If we agree with your feature proposal, we'll work with you to develop and integrate the feature. But please bear with us, as we may not always be able to respond immediately.

* Please avoid making commits directly to your copy of the `master` branch. This branch is reserved for aggregating changes from other people, and for mainline development from the core contributors. If you commit to `master`, it's likely that your local fork will diverge from the [upstream repository](https://github.com/bbc/audiowaveform).

* Before working on a change, please ensure your local fork is up to date with the code in the upstream repository, and create a [feature branch](https://www.atlassian.com/git/tutorials/comparing-workflows/feature-branch-workflow) for your changes.

* Please don't change the [VERSION](https://github.com/bbc/audiowaveform/blob/master/VERSION) file, or update the [ChangeLog](https://github.com/bbc/audiowaveform/blob/master/ChangeLog). We'll do that when [preparing a new release](#preparing-a-new-release).

* Please follow the existing coding conventions.

* For commit messages, please follow [these guidelines](https://chris.beams.io/posts/git-commit/), although we're not fussy about use of imperative mood vs past tense. In particular, avoid commit messages that include [Angular-style metadata](https://github.com/angular/angular/blob/master/CONTRIBUTING.md#-commit-message-guidelines).

* Please add test cases for your feature, and ensure all tests are passing (`make test`).

* When merging a feature branch, core contributors may choose to squash your commits, so that the feature is merged as a single logical change.

### Preparing a new release

When it's time to publish a new release version, create a single commit on `master` with the following changes only:

* Increment the version number in [VERSION](https://github.com/bbc/audiowaveform/blob/master/VERSION)
* Describe the new features in this release in [ChangeLog](https://github.com/bbc/audiowaveform/blob/master/ChangeLog)
* Update the [debian changelog](https://github.com/bbc/audiowaveform/blob/master/debian/changelog)

Tag this commit using the form `X.Y.Z` and push the commit using `git push origin master --tags`.

* Publish the source package to [Launchpad](https://launchpad.net/)
* Update the [Homebrew formula](https://github.com/bbc/homebrew-audiowaveform)
