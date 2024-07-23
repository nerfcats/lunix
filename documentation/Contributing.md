# Contributing to Lunix
## Introduction

> [!NOTE]
> This part of the documentation is a work in progress

Thank you for considering to contribute to Lunix.

**Working on your first Pull Request?** You can learn how from this *free* series [How to Contribute to an Open Source Project on GitHub](https://kcd.im/pull-request) 

## Code of Conduct
Please read and follow our [Code of Conduct](/CODE_OF_CONDUCT.md).

## Getting Started
### Prerequisites
- Linux or WSL (if you are on Windows)
- `cmake make gcc g++ libssl-dev/openssl-devel`

### Installation
1. Clone the repo: `git clone https://github.com/noahdossan/lunix.git`
2. Build Lunix: `./Build.sh`

## How to Contribute
### Types of Contributions
- Reporting bugs
- Suggesting features
- Submitting code
- Improving documentation

### Workflow
1. Fork the repository.
2. Create a branch: `git checkout -b feature/your-feature-name`
3. Make your changes.
4. Commit your changes: `git commit -m 'feat(scope): add something'`
5. Push to your branch: `git push origin feature/your-feature-name`
6. Submit a pull request.

## Style Guides
### Coding Standards
Please follow [coding standard](http://micro-os-plus.github.io/develop/sutter-101/) for C++.

### Documentation Standards
Use [Markdown syntax guide](MarkdownGuide.md) for documentation.

### Commit Messages
Use the following format: `type(scope): message`. A valid commit message: `docs: add something to README.md`.

Types of commits are defined in `commitlint.config.ts`.

## Issue Tracking
### Creating Issues
When creating an issue, please include:
- A clear and descriptive title
- A detailed description
- Steps to reproduce the issue

<!--
## Community and Support
Join our [discussion forum](link_to_forum) or reach out on [chat platform](link_to_chat).
-->

## Licensing and Legal
By contributing, you agree that your contributions will be licensed under GPL v3.

Thank you for your contributions!
