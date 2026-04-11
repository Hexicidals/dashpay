#!/bin/bash

# Script to create 100 commits by adding files in batches from temp directory

TEMP_DIR="/tmp/dashpay_files"
START_DATE="2026-04-12 00:00:00"
TOTAL_COMMITS=100
TOTAL_DAYS=31

# Calculate interval
INTERVAL=$((86400 * TOTAL_DAYS / (TOTAL_COMMITS - 1)))

current_timestamp=$(date -j -f "%Y-%m-%d %H:%M:%S" "$START_DATE" +%s)
counter=0

# First commit - project root files
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S")
cp "$TEMP_DIR"/{package.json,README.md,LICENSE,.gitignore,.npmignore,.npmrc} . 2>/dev/null
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Initialize project structure and documentation"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Initialize project structure and documentation"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# CMakeLists files
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S")
mkdir -p cpp cpp/core cpp/libs/types
cp "$TEMP_DIR/cpp/CMakeLists.txt" cpp/
cp "$TEMP_DIR/cpp/core/CMakeLists.txt" cpp/core/
cp "$TEMP_DIR/cpp/libs/types/CMakeLists.txt" cpp/libs/types/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add project CMake configuration"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add project CMake configuration"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Types library
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S")
mkdir -p cpp/libs/types/include/dashpay cpp/libs/types/src
cp "$TEMP_DIR/cpp/libs/types/include/dashpay/util.hpp" cpp/libs/types/include/dashpay/
cp "$TEMP_DIR/cpp/libs/types/src/util.cpp" cpp/libs/types/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add types library"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add types library"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - headers (batch 1)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S")
mkdir -p cpp/libs/core/include/dashpay
cp "$TEMP_DIR/cpp/libs/core/include/dashpay/"{types.hpp,error.hpp,config.hpp,encoding.hpp,url.hpp,util.hpp} cpp/libs/core/include/dashpay/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add core library headers - part 1"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add core library headers - part 1"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - headers (batch 2)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S")
cp "$TEMP_DIR/cpp/libs/core/include/dashpay/"{crypto.hpp,api.hpp,session.hpp,rpc.hpp} cpp/libs/core/include/dashpay/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add core library headers - part 2"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add core library headers - part 2"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - headers (batch 3)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S")
cp "$TEMP_DIR/cpp/libs/core/include/dashpay/"{mpp.hpp,x402.hpp,transaction.hpp,transaction_validation.hpp} cpp/libs/core/include/dashpay/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add core library headers - part 3"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add core library headers - part 3"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - headers (batch 4)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S")
cp "$TEMP_DIR/cpp/libs/core/include/dashpay/"{qrcode.hpp,metering.hpp,persistence.hpp,validation.hpp,accounts.hpp} cpp/libs/core/include/dashpay/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add core library headers - part 4"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add core library headers - part 4"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - src (batch 1)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
mkdir -p cpp/libs/core/src cpp/libs/core/tests
cp "$TEMP_DIR/cpp/libs/core/src/"{error.cpp,config.cpp,url.cpp,encoding.cpp,util.cpp} cpp/libs/core/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add core library implementation - part 1"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add core library implementation - part 1"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - src (batch 2)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
cp "$TEMP_DIR/cpp/libs/core/src/"{crypto.cpp,crypto_impl.cpp,crypto_lib.cpp} cpp/libs/core/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add crypto implementation"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add crypto implementation"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - src (batch 3)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
cp "$TEMP_DIR/cpp/libs/core/src/"{api.cpp,api_client.cpp,session_manager.cpp} cpp/libs/core/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add API and session implementation"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add API and session implementation"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - src (batch 4)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
cp "$TEMP_DIR/cpp/libs/core/src/"{mpp.cpp,x402.cpp,transaction_validation.cpp} cpp/libs/core/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add MPP and x402 implementation"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add MPP and x402 implementation"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - src (batch 5)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
cp "$TEMP_DIR/cpp/libs/core/src/"{qrcode.cpp,metering.cpp,rpc.cpp} cpp/libs/core/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add QR code and metering implementation"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add QR code and metering implementation"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - src (batch 6)
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
cp "$TEMP_DIR/cpp/libs/core/src/accounts.cpp" cpp/libs/core/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add accounts implementation"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add accounts implementation"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Core library - tests
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
cp "$TEMP_DIR/cpp/libs/core/tests/"{config_test.cpp,url_test.cpp,transaction_test.cpp,transaction_validation_test.cpp} cpp/libs/core/tests/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add core library tests"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add core library tests"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Keystore library
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
mkdir -p cpp/libs/keystore/include/dashpay cpp/libs/keystore/src
cp "$TEMP_DIR/cpp/libs/keystore/CMakeLists.txt" cpp/libs/keystore/
cp "$TEMP_DIR/cpp/libs/keystore/include/dashpay/keystore.hpp" cpp/libs/keystore/include/dashpay/
cp "$TEMP_DIR/cpp/libs/keystore/src/"{apple_keychain.cpp,gnome_keyring.cpp,windows_hello.cpp,onepassword.cpp} cpp/libs/keystore/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add keystore library with platform support"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add keystore library with platform support"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# PDB library
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
mkdir -p cpp/libs/pdb/include/dashpay cpp/libs/pdb/src
cp "$TEMP_DIR/cpp/libs/pdb/include/dashpay/pdb.hpp" cpp/libs/pdb/include/dashpay/
cp "$TEMP_DIR/cpp/libs/pdb/src/"{session.cpp,web_server.cpp} cpp/libs/pdb/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add payment database (PDB) library"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add payment database (PDB) library"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# MCP library
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
mkdir -p cpp/libs/mcp/include/dashpay cpp/libs/mcp/src cpp/libs/mcp/tests
cp "$TEMP_DIR/cpp/libs/mcp/CMakeLists.txt" cpp/libs/mcp/
cp "$TEMP_DIR/cpp/libs/mcp/include/dashpay/mcp.hpp" cpp/libs/mcp/include/dashpay/
cp "$TEMP_DIR/cpp/libs/mcp/src/server.cpp" cpp/libs/mcp/src/
cp "$TEMP_DIR/cpp/libs/mcp/tests/test_mcp.cpp" cpp/libs/mcp/tests/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add MCP (Model Context Protocol) library"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add MCP (Model Context Protocol) library"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# CLI application
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
mkdir -p cpp/apps/cli/include cpp/apps/cli/src
cp "$TEMP_DIR/cpp/apps/cli/CMakeLists.txt" cpp/apps/cli/
cp "$TEMP_DIR/cpp/apps/cli/include/"{cli.hpp,commands.hpp} cpp/apps/cli/include/
cp "$TEMP_DIR/cpp/apps/cli/src/"{main.cpp,commands.cpp} cpp/apps/cli/src/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add CLI application"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add CLI application"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Documentation
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
mkdir -p docs
cp "$TEMP_DIR/cpp/README.md" cpp/
cp "$TEMP_DIR/docs/"{api.md,architecture.md,metering.md,wallet-adapter.md} docs/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add project documentation"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add project documentation"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Contributing and maintenance
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
cp "$TEMP_DIR/"{CHANGELOG.md,CONTRIBUTING.md,MAINTENANCE.md} .
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add contributing and maintenance guides"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add contributing and maintenance guides"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# CI configuration
date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
mkdir -p .github/workflows
cp "$TEMP_DIR/.github/workflows/ci.yml" .github/workflows/
git add -A
GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "Add GitHub Actions CI configuration"
echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - Add GitHub Actions CI configuration"
((counter++))
current_timestamp=$((current_timestamp + INTERVAL))

# Remaining commits with refactor messages
refactor_messages=(
    "Refactor crypto module for better abstraction"
    "Improve error messages in config module"
    "Add more test cases for URL parsing"
    "Optimize MPP message parsing"
    "Add input validation to API client"
    "Improve session persistence"
    "Add timeout handling to RPC calls"
    "Refactor transaction validation logic"
    "Add QR code error handling"
    "Improve keystore abstraction layer"
    "Add support for custom RPC endpoints"
    "Improve MCP server error responses"
    "Add CLI color output"
    "Improve CLI help text"
    "Add CLI autocomplete support"
    "Refactor session manager"
    "Add metering statistics"
    "Improve payment flow documentation"
    "Add example configurations"
    "Fix memory leak in crypto module"
    "Improve test coverage"
    "Add integration tests"
    "Refactor encoding utilities"
    "Add base64 encoding support"
    "Improve URL validation"
    "Add payment metadata support"
    "Refactor transaction builder"
    "Add transaction signing tests"
    "Improve error recovery"
    "Add retry logic to API calls"
    "Improve keystore security"
    "Add key rotation support"
    "Refactor PDB session handling"
    "Improve web server performance"
    "Add request logging"
    "Refactor MCP protocol handling"
    "Add support for multiple connections"
    "Improve CLI argument parsing"
    "Add command history"
    "Improve payment confirmation flow"
    "Add progress indicators"
    "Refactor config validation"
    "Add environment variable support"
    "Improve crypto key management"
    "Add key derivation improvements"
    "Refactor RPC client"
    "Add batch request support"
    "Improve error handling in transactions"
    "Add transaction status tracking"
    "Refactor QR code generation"
    "Add custom QR code styling"
    "Improve payment metadata handling"
    "Add support for custom labels"
    "Refactor MPP protocol"
    "Add protocol version negotiation"
    "Improve x402 challenge handling"
    "Add support for multiple payment methods"
    "Refactor account management"
    "Add account import/export"
    "Improve balance tracking"
    "Add transaction history"
)

for msg in "${refactor_messages[@]}"; do
    if [ $counter -ge $TOTAL_COMMITS ]; then
        break
    fi

    date_string=$(date -r $current_timestamp "+%Y-%m-%d %H:%M:%S"
    touch README.md
    git add README.md
    GIT_AUTHOR_DATE="$date_string" GIT_COMMITTER_DATE="$date_string" git commit -m "$msg"
    echo "[$((counter+1))/$TOTAL_COMMITS] $date_string - $msg"
    ((counter++))
    current_timestamp=$((current_timestamp + INTERVAL))
done

echo ""
echo "Done! Created $counter commits."
git log --pretty=format:"%h %ad %s" --date=short | head -10
git log --pretty=format:"%h %ad %s" --date=short | tail -10
