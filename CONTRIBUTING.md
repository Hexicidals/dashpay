# Contributing to DashPay

Thank you for your interest in contributing to DashPay!

## Development Setup

1. Fork the repository
2. Clone your fork: `git clone https://github.com/your-username/dashpay.git`
3. Create a branch: `git checkout -b feature/your-feature`

## Building

```sh
mkdir build && cd build
cmake ..
make
```

## Running Tests

```sh
ctest --test-dir build
```

## Code Style

- C++20 standard
- Use `[[nodiscard]]` attribute where appropriate
- Use `std::expected<T, E>` instead of exceptions
- Use `std::string_view` for string parameters
- Use `auto` with explicit types where readability improves

## Commit Messages

Follow conventional commits:
- `feat:` new features
- `fix:` bug fixes
- `docs:` documentation changes
- `test:` test changes
- `refactor:` refactoring
- `chore:` maintenance tasks

## Pull Requests

1. Ensure all tests pass
2. Update documentation if needed
3. Add tests for new functionality
4. Keep PRs focused on a single issue or feature

## Licensing

By contributing, you agree that your contributions will be licensed under the MIT License.
