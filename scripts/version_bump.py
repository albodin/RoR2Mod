#!/usr/bin/env python3
"""
Version bumping script for RoR2Mod
Automatically bumps version based on commit message keywords:
- "BREAKING CHANGE", "!:" -> major version
- "feat:", "add:" -> minor version  
- "fix:", "patch:", "bugfix:" -> patch version
- Otherwise -> build number only
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
        return 1, 0, 0, 0
    
    with open(version_file, 'r') as f:
        content = f.read()
    
    major = int(re.search(r'#define VERSION_MAJOR (\d+)', content).group(1))
    minor = int(re.search(r'#define VERSION_MINOR (\d+)', content).group(1))
    patch = int(re.search(r'#define VERSION_PATCH (\d+)', content).group(1))
    build = int(re.search(r'#define VERSION_BUILD (\d+)', content).group(1))
    
    return major, minor, patch, build

def write_version_file(major, minor, patch, build):
    """Write new version to version.h"""
    version_file = get_project_root() / "src" / "version.h"
    version_string = f"{major}.{minor}.{patch}"
    version_string_full = f"{major}.{minor}.{patch}.{build}"
    
    content = f"""#pragma once

#define VERSION_MAJOR {major}
#define VERSION_MINOR {minor}
#define VERSION_PATCH {patch}
#define VERSION_BUILD {build}

#define VERSION_STRING "{version_string}"
#define VERSION_STRING_FULL "{version_string_full}"

// Auto-generated version info
// This file is automatically updated by version_bump.py
"""
    
    with open(version_file, 'w') as f:
        f.write(content)
    
    print(f"Version updated to {version_string} (build {build})")
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
    commit_msg_file = get_project_root() / ".git" / "COMMIT_EDITMSG"
    if commit_msg_file.exists():
        with open(commit_msg_file, 'r') as f:
            return f.read().strip()
    return ""

def analyze_commit_message(message):
    """Analyze commit message to determine version bump type"""
    # Convert to lowercase for case-insensitive matching
    msg_lower = message.lower()
    
    # Check for breaking changes (major version bump)
    if 'breaking change' in msg_lower or '!:' in message:
        return 'major'
    
    # Check for features/additions (minor version bump)
    feature_patterns = [
        r'^feat:', r'^feature:', r'^add:', r'^added:',
        r'\bfeature\b', r'\badds?\b', r'\bimplement', r'\bnew\b'
    ]
    for pattern in feature_patterns:
        if re.search(pattern, msg_lower):
            return 'minor'
    
    # Check for fixes (patch version bump)
    fix_patterns = [
        r'^fix:', r'^fixed:', r'^patch:', r'^bugfix:',
        r'\bfix(?:es|ed)?\b', r'\bbugfix', r'\bpatch\b', r'\brepair'
    ]
    for pattern in fix_patterns:
        if re.search(pattern, msg_lower):
            return 'patch'
    
    # Default to build bump only
    return 'build'

def main():
    """Main version bumping logic"""
    major, minor, patch, build = read_version_file()
    
    if len(sys.argv) > 1:
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
        elif bump_type == 'build':
            # Only increment build for CI/CD purposes
            build += 1
        elif bump_type == 'auto':
            # Force auto mode
            pass
        else:
            print(f"Usage: {sys.argv[0]} [major|minor|patch|build|auto]")
            print(f"Current version: {major}.{minor}.{patch}")
            return
    
    # Auto-detect version bump from commit message (when no args or 'auto')
    if len(sys.argv) <= 1 or (len(sys.argv) > 1 and sys.argv[1].lower() == 'auto'):
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
                print(f"No version change needed for this commit")
                return
    
    version_string = write_version_file(major, minor, patch, build)
    
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