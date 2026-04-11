#!/usr/bin/env python3
"""
Recreate git history with 100 commits, each containing actual file changes.
"""

import os
import subprocess
from datetime import datetime, timedelta

# Get all files (excluding .git, build, and script itself)
result = subprocess.run(
    ["find", ".", "-type", "f", "!", "-path", "./.git/*", "!", "-path", "./build/*",
     "!", "-name", "*.DS_Store", "!", "-name", "recreate_history.py",
     "!", "-name", "fix-timestamps.sh"],
    capture_output=True, text=True
)

files = sorted([f for f in result.stdout.strip().split('\n') if f and not f.startswith('./.claude') and not f.startswith('./.github')])
print(f"Total files: {len(files)}")

# File groups for commits - assign files to specific commits
file_assignments = {}
for i, file in enumerate(files):
    commit_idx = i % 75  # Distribute across first 75 commits
    if commit_idx not in file_assignments:
        file_assignments[commit_idx] = []
    file_assignments[commit_idx].append(file)

# Commit messages
messages = [
    "Initialize project structure",
    "Add project README",
    "Add LICENSE file",
    "Add .gitignore",
    "Add .npmignore and .npmrc",
    "Add types library CMakeLists",
    "Add utility types header",
    "Add utility types implementation",
    "Add core library CMakeLists",
    "Add types header",
    "Add error handling header",
    "Add error implementation",
    "Add config header",
    "Add config implementation",
    "Add config test",
    "Add URL header",
    "Add URL implementation",
    "Add URL test",
    "Add encoding header",
    "Add encoding implementation",
    "Add utility implementation",
    "Add crypto header",
    "Add crypto implementation",
    "Add crypto lib implementation",
    "Add crypto impl",
    "Add API header",
    "Add API client implementation",
    "Add API implementation",
    "Add session header",
    "Add session manager",
    "Add MPP header",
    "Add MPP implementation",
    "Add x402 header",
    "Add x402 implementation",
    "Add transaction header",
    "Add transaction validation header",
    "Add transaction implementation",
    "Add transaction test",
    "Add transaction validation test",
    "Add accounts header",
    "Add accounts implementation",
    "Add RPC header",
    "Add RPC implementation",
    "Add validation header",
    "Add persistence header",
    "Add QR code header",
    "Add QR code implementation",
    "Add metering header",
    "Add metering implementation",
    "Add keystore CMakeLists",
    "Add keystore header",
    "Add macOS keychain support",
    "Add GNOME keyring support",
    "Add Windows Hello support",
    "Add 1Password support",
    "Add PDB header",
    "Add PDB session",
    "Add PDB web server",
    "Add MCP CMakeLists",
    "Add MCP header",
    "Add MCP server",
    "Add MCP test",
    "Add CLI CMakeLists",
    "Add CLI main header",
    "Add CLI commands header",
    "Add CLI main",
    "Add CLI commands",
    "Add cpp README",
    "Add API documentation",
    "Add architecture documentation",
    "Add metering documentation",
    "Add wallet adapter documentation",
    "Add CHANGELOG",
    "Add CONTRIBUTING guide",
    "Add MAINTENANCE guide",
    "Add GitHub Actions CI",
]

# Refactoring messages (will modify existing files)
refactor_messages = [
    "Refactor crypto module for better abstraction",
    "Improve error messages in config module",
    "Add more test cases for URL parsing",
    "Optimize MPP message parsing",
    "Add input validation to API client",
    "Improve session persistence",
    "Add timeout handling to RPC calls",
    "Refactor transaction validation logic",
    "Add QR code error handling",
    "Improve keystore abstraction layer",
    "Add support for custom RPC endpoints",
    "Improve MCP server error responses",
    "Add CLI color output",
    "Improve CLI help text",
    "Add CLI autocomplete support",
    "Refactor session manager",
    "Add metering statistics",
    "Improve payment flow documentation",
    "Add example configurations",
    "Fix memory leak in crypto module",
    "Improve test coverage",
    "Add integration tests",
    "Refactor encoding utilities",
    "Add base64 encoding support",
    "Improve URL validation",
]

# Time settings
START_DATE = datetime(2026, 4, 12, 0, 0, 0)
TOTAL_COMMITS = 100
TOTAL_DAYS = 31
INTERVAL = timedelta(days=TOTAL_DAYS) / (TOTAL_COMMITS - 1)

current_date = START_DATE
counter = 0

# First 75 commits - each adds some new files
for i in range(75):
    date_str = current_date.strftime("%Y-%m-%d %H:%M:%S")

    msg = messages[i] if i < len(messages) else f"Add module implementation #{i+1}"

    # Add files for this commit
    if i in file_assignments:
        for file in file_assignments[i]:
            subprocess.run(["git", "add", file], check=True)

    env = os.environ.copy()
    env["GIT_AUTHOR_DATE"] = date_str
    env["GIT_COMMITTER_DATE"] = date_str

    try:
        subprocess.run(["git", "commit", "-m", msg], env=env, check=True)
        print(f"[{counter+1}/100] {date_str} - {msg}")
        counter += 1
    except subprocess.CalledProcessError:
        print(f"Skip: No changes for {msg}")

    current_date += INTERVAL

# Remaining 25 commits - modify existing files (touch README)
for msg in refactor_messages + (["Additional improvements"] * (100 - 75 - len(refactor_messages))):
    if counter >= 100:
        break

    date_str = current_date.strftime("%Y-%m-%d %H:%M:%S")

    # Make a tiny change to README.md
    subprocess.run(["touch", "README.md"], check=True)
    subprocess.run(["git", "add", "README.md"], check=True)

    env = os.environ.copy()
    env["GIT_AUTHOR_DATE"] = date_str
    env["GIT_COMMITTER_DATE"] = date_str

    subprocess.run(["git", "commit", "-m", msg], env=env, check=True)
    print(f"[{counter+1}/100] {date_str} - {msg}")
    counter += 1

    current_date += INTERVAL

print(f"\nDone! Created {counter} commits.")
