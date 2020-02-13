# Changelog

All notable changes to DashPay will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

### Added
- Initial C++ implementation of payment protocols
- MPP (Machine Payments Protocol) support
- x402 payment protocol support
- Keystore backends (Apple Keychain, Windows Hello, GNOME Keyring, 1Password)
- MCP (Model Context Protocol) server
- Payment Debugger (PDB) web UI
- CLI with all payment commands
- Session management and accounting

### Changed
- Rebranding from Solana Pay to DashPay
- All references to "pay" updated to "dashpay"
- Repository structure adapted for C++ build system

### Fixed
- N/A (initial release)

### Security
- All key storage uses platform-native secure vaults
- No secrets logged or included in git history
