#!/usr/bin/env python3
"""
Version bumping script for RoR2Mod
Automatically bumps version based on commit message keywords:
- "BREAKING CHANGE", "!:" -> major version
- "feat:", "add:" -> minor version  
- "fix:", "patch:", "bugfix:" -> patch version
- Otherwise -> no version change
"""

import os
import sys
import re
import subprocess
from pathlib import Path

def get_project_root():
    """Get the project root directory"""
    script_dir = Path(__file__).parent
    return script_dir.parent

def read_version_file():
    """Read current version from version.h"""
    version_file = get_project_root() / "src" / "version.h"
    
    if not version_file.exists():
        return 1, 0, 0
    
    with open(version_file, 'r') as f:
        content = f.read()
    
    major = int(re.search(r'#define VERSION_MAJOR (\d+)', content).group(1))
    minor = int(re.search(r'#define VERSION_MINOR (\d+)', content).group(1))
    patch = int(re.search(r'#define VERSION_PATCH (\d+)', content).group(1))
    
    return major, minor, patch

def write_version_file(major, minor, patch):
    """Write new version to version.h"""
    version_file = get_project_root() / "src" / "version.h"
    version_string = f"{major}.{minor}.{patch}"
    
    content = f"""#pragma once

#define VERSION_MAJOR {major}
#define VERSION_MINOR {minor}
#define VERSION_PATCH {patch}

#define VERSION_STRING "{version_string}"

// Auto-generated version info
// This file is automatically updated by version_bump.py
"""
    
    with open(version_file, 'w') as f:
        f.write(content)
    
    print(f"Version updated to {version_string}")
    return version_string

def is_git_dirty():
    """Check if there are uncommitted changes"""
    try:
        result = subprocess.run(['git', 'diff', '--quiet'], 
                              cwd=get_project_root(), 
                              capture_output=True)
        return result.returncode != 0
    except:
        return False

def get_commit_count():
    """Get the number of commits in the current branch"""
    try:
        result = subprocess.run(['git', 'rev-list', '--count', 'HEAD'], 
                              cwd=get_project_root(), 
                              capture_output=True, text=True)
        if result.returncode == 0:
            return int(result.stdout.strip())
    except:
        pass
    return 0

def get_staged_commit_message():
    """Get the commit message being prepared"""
    # Debug logging
    debug_file = get_project_root() / ".git" / "version_bump_debug.log"
    with open(debug_file, 'a') as f:
        f.write(f"\n[{sys.argv[0]}] Called at {subprocess.run(['date'], capture_output=True, text=True).stdout.strip()}\n")
        f.write(f"stdin.isatty(): {sys.stdin.isatty()}\n")
    
    # First try to read from stdin if available
    if not sys.stdin.isatty():
        try:
            msg = sys.stdin.read().strip()
            with open(debug_file, 'a') as f:
                f.write(f"Got message from stdin: '{msg}'\n")
            return msg
        except Exception as e:
            with open(debug_file, 'a') as f:
                f.write(f"Failed to read from stdin: {e}\n")
    
    # Fallback to COMMIT_EDITMSG file
    commit_msg_file = get_project_root() / ".git" / "COMMIT_EDITMSG"
    if commit_msg_file.exists():
        with open(commit_msg_file, 'r') as f:
            msg = f.read().strip()
            with open(debug_file, 'a') as f:
                f.write(f"Got message from COMMIT_EDITMSG: '{msg}'\n")
            return msg
    
    with open(debug_file, 'a') as f:
        f.write("No commit message found\n")
    return ""

def analyze_commit_message(message):
    """Analyze commit message to determine version bump type"""
    # Convert to lowercase for case-insensitive matching
    msg_lower = message.lower()
    
    # Check for breaking changes (major version bump)
    if 'breaking change' in msg_lower or '!:' in message:
        return 'major'
    
    # Check for fixes first (patch version bump) - more specific patterns
    fix_patterns = [
        r'^fix:', r'^fixed:', r'^patch:', r'^bugfix:'
    ]
    for pattern in fix_patterns:
        if re.search(pattern, msg_lower):
            return 'patch'
    
    # Check for features/additions (minor version bump)
    feature_patterns = [
        r'^feat:', r'^feature:', r'^add:', r'^added:'
    ]
    for pattern in feature_patterns:
        if re.search(pattern, msg_lower):
            return 'minor'
    
    # Default to no version change
    return 'none'

def main():
    """Main version bumping logic"""
    major, minor, patch = read_version_file()
    
    # Determine if we should use auto mode
    use_auto_mode = len(sys.argv) <= 1 or (len(sys.argv) > 1 and sys.argv[1].lower() == 'auto')
    
    if use_auto_mode:
        # Auto-detect version bump from commit message
        commit_msg = get_staged_commit_message()
        
        if commit_msg:
            bump_type = analyze_commit_message(commit_msg)
            
            if bump_type == 'major':
                print(f"Detected MAJOR version bump from commit message")
                major += 1
                minor = 0
                patch = 0
            elif bump_type == 'minor':
                print(f"Detected MINOR version bump from commit message")
                minor += 1
                patch = 0
            elif bump_type == 'patch':
                print(f"Detected PATCH version bump from commit message")
                patch += 1
            else:
                # For regular commits, don't change version
                print(f"No semantic version keywords found - no version change needed")
                return
        else:
            print("No commit message found")
            return
    else:
        # Manual version bumping
        bump_type = sys.argv[1].lower()
        
        if bump_type == 'major':
            major += 1
            minor = 0
            patch = 0
        elif bump_type == 'minor':
            minor += 1
            patch = 0
        elif bump_type == 'patch':
            patch += 1
        else:
            print(f"Usage: {sys.argv[0]} [major|minor|patch|auto]")
            print(f"Current version: {major}.{minor}.{patch}")
            return
    
    version_string = write_version_file(major, minor, patch)
    
    # If this is part of a commit, stage the version file
    if is_git_dirty():
        try:
            subprocess.run(['git', 'add', 'src/version.h'], 
                         cwd=get_project_root(), 
                         check=True)
            print("Version file staged for commit")
        except:
            print("Warning: Could not stage version file")

if __name__ == '__main__':
    main()