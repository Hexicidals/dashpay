# Nix Configuration

## Overview

DashPay can be built using Nix for reproducible development environments.

## Example flake.nix

```nix
{
  description = "DashPay CLI";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = {
    default = {
      pkgs = import nixpkgs {
        inherit (pkgs) system;
        buildInputs = with pkgs; [
          cmake
          ninja
          openssl.dev
          pkg-config
        ];
      };
    };
  };
}
```

## Building

```sh
nix-build
nix develop
```

## Shell Integration

Add to your `shell.nix`:

```nix
{ pkgs ? import <nixpkgs> {} }:
  {
    environment.systemPackages = with pkgs; [
      dashpay
    ];
  }
}
```
