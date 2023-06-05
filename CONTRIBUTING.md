# Contributing

Thank you for your interest in audiowaveform!

We love hearing feedback from people who use our software, so if you are using this for something interesting, please let us know.

Contributions are welcomed and encouraged. If you're thinking of writing a new feature, please first discuss the change you wish to make, either by raising an issue, or contacting us directly, e.g., [by email](mailto:chris@chrisneedham.com).

We may not always be able to respond immediately to feedback, so please bear with us and have patience.

## Making changes

* If we agree with your feature proposal, we'll work with you to develop and integrate the feature.

* Please avoid making commits directly to your copy of the `master` branch. This branch is reserved for aggregating changes from other people, and for mainline development from the core contributors. If you commit to `master`, it's likely that your local fork will diverge from the [upstream repository](https://github.com/bbc/audiowaveform).

* Before working on a change, please ensure your local fork is up to date with the code in the upstream repository, and create a [feature branch](https://www.atlassian.com/git/tutorials/comparing-workflows/feature-branch-workflow) for your changes.

* We may want to make minor changes to your pull request before merging, so please ensure that the **Allow edits from maintainers** option on your feature branch is enabled.

* Please don't change the [VERSION](https://github.com/bbc/audiowaveform/blob/master/VERSION) file, or update the [ChangeLog](https://github.com/bbc/audiowaveform/blob/master/ChangeLog). We'll do that when [preparing a new release](#preparing-a-new-release).

* Please follow the existing coding conventions.

* For commit messages, please follow [these guidelines](https://chris.beams.io/posts/git-commit/), although we're not fussy about use of imperative mood vs past tense. In particular, please don't use [Conventional Commits](https://www.conventionalcommits.org/) style. We may choose to edit your commit messages for consistency when merging.

* Please add test cases for your feature, and ensure all tests are passing (`make test`).

* When merging a feature branch, core contributors may choose to squash your commits, so that the feature is merged as a single logical change.

### Preparing a new release

* When it's time to publish a new release version, create a single commit on `master` with the following changes only:

  * Increment the version number in [VERSION](https://github.com/bbc/audiowaveform/blob/master/VERSION).

  * Describe the new features in this release in [CHANGELOG.md](https://github.com/bbc/audiowaveform/blob/master/CHANGELOG.md).

  * Update the [debian changelog](https://github.com/bbc/audiowaveform/blob/master/debian/changelog).

* Tag this commit using the form `X.Y.Z` and push the commit using `git push origin master --tags`.

* In GitHub, [create a Release](https://github.com/bbc/audiowaveform/releases/new) from this tag, with the tag name as Release title, i.e., `X.Y.Z`.

* Publish the source package to [Launchpad](https://launchpad.net/) using the [packaging script](https://github.com/bbc/audiowaveform/tree/master/ubuntu).

* Update the [Homebrew formula](https://github.com/bbc/homebrew-audiowaveform).

* Compile Windows binaries using [compile-static-audiowaveform](https://github.com/chrisn/compile-static-audiowaveform) and upload to the GitHub Release page.

* Build [RPM packages](https://github.com/bbc/audiowaveform/tree/master/rpm) and upload to the GitHub Release page.
