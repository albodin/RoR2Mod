#!/usr/bin/env python3
"""
Script to update GameStructs.h based on the latest RoR2.h dump.
Compares structs and updates them if they don't match.
"""

import re
import sys
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import argparse
import logging
import difflib

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)

# Type normalization mapping: types that should be renamed when reading from RoR2.h
TYPE_NORMALIZATIONS = {
    "Vector3_Value": "Vector3",
    "Vector2_Value": "Vector2",
    "Quaternion_Value": "Quaternion",
    "Rect_Value": "Rect",
    # Add more type mappings here as needed
}

# ANSI color codes
class Colors:
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    WHITE = '\033[97m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

    @staticmethod
    def colorize(text: str, color: str) -> str:
        """Colorize text with ANSI escape codes."""
        return f"{color}{text}{Colors.RESET}"

    @staticmethod
    def red(text: str) -> str:
        return Colors.colorize(text, Colors.RED)

    @staticmethod
    def green(text: str) -> str:
        return Colors.colorize(text, Colors.GREEN)

    @staticmethod
    def yellow(text: str) -> str:
        return Colors.colorize(text, Colors.YELLOW)

    @staticmethod
    def cyan(text: str) -> str:
        return Colors.colorize(text, Colors.CYAN)

    @staticmethod
    def bold(text: str) -> str:
        return Colors.colorize(text, Colors.BOLD)

class Struct:
    def __init__(self, name: str, members: List[Tuple[str, str, str]], generation_comment: str = ""):
        self.name = name
        self.members = members  # List of (type, name, comment/offset)
        self.generation_comment = generation_comment
        self.is_value_struct = name.endswith("_Value")

    def __str__(self):
        lines = []
        if self.generation_comment:
            lines.append(self.generation_comment)
        lines.append(f"struct {self.name} {{")
        for member_type, member_name, comment in self.members:
            if comment:
                lines.append(f"    {member_type} {member_name}; {comment}")
            else:
                lines.append(f"    {member_type} {member_name};")
        lines.append("};")
        return "\n".join(lines)

    def normalize_for_comparison(self) -> List[Tuple[str, str]]:
        """Normalize struct for comparison, removing comments and handling _Value structs."""
        normalized = []
        for i, (member_type, member_name, comment) in enumerate(self.members):
            # Skip first padding for _Value structs
            if self.is_value_struct and i == 0 and "padding" in member_name:
                continue
            # For _Value structs, ignore offset comments
            if self.is_value_struct:
                normalized.append((member_type, member_name))
            else:
                # For regular structs, include the offset comment in comparison
                normalized.append((member_type, member_name))
        return normalized

class StructParser:
    def __init__(self):
        self.struct_pattern = re.compile(r'^struct\s+(\w+)\s*\{', re.MULTILINE)
        self.member_pattern = re.compile(r'^\s*(.+?)\s+([^\s;]+);\s*(.*)$')
        self.generation_comment_pattern = re.compile(r'^//\s*Generated from\s+(.+)$', re.MULTILINE)

    def parse_file(self, filepath: Path, normalize_vector_types: bool = False) -> Dict[str, Struct]:
        """Parse a C++ header file and extract struct definitions."""
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()

        structs = {}

        # Find all struct definitions
        for match in self.struct_pattern.finditer(content):
            struct_name = match.group(1)
            start_pos = match.start()

            # Find the generation comment before the struct
            generation_comment = ""
            # Look backwards line by line to find the most recent generation comment
            lines_before = content[:start_pos].split('\n')
            for line in reversed(lines_before[-10:]):  # Check last 10 lines
                line = line.strip()
                gen_match = self.generation_comment_pattern.match(line)
                if gen_match:
                    generation_comment = f"// Generated from {gen_match.group(1)}"
                    break
                elif line and not line.startswith('//'):
                    # Stop if we hit non-comment, non-empty content
                    break

            # Find the closing brace
            brace_count = 0
            pos = match.end()
            struct_start = pos

            while pos < len(content):
                if content[pos] == '{':
                    brace_count += 1
                elif content[pos] == '}':
                    if brace_count == 0:
                        struct_end = pos
                        break
                    brace_count -= 1
                pos += 1
            else:
                logger.warning(f"Could not find closing brace for struct {struct_name}")
                continue

            # Extract struct body
            struct_body = content[struct_start:struct_end]

            # Parse members
            members = []
            for line in struct_body.split('\n'):
                line = line.strip()
                if not line or line.startswith('//'):
                    continue

                member_match = self.member_pattern.match(line)
                if member_match:
                    member_type = member_match.group(1).strip()
                    member_name = member_match.group(2).strip()
                    comment = member_match.group(3).strip()

                    # Normalize types based on the normalization mapping
                    if normalize_vector_types and member_type in TYPE_NORMALIZATIONS:
                        member_type = TYPE_NORMALIZATIONS[member_type]

                    members.append((member_type, member_name, comment))

            structs[struct_name] = Struct(struct_name, members, generation_comment)

        return structs

class StructUpdater:
    def __init__(self, game_dump_path: Path, game_structs_path: Path):
        self.game_dump_path = game_dump_path / "RoR2.h"
        self.game_structs_path = game_structs_path
        self.parser = StructParser()

    def load_structs(self) -> Tuple[Dict[str, Struct], Dict[str, Struct]]:
        """Load structs from both files."""
        if not self.game_dump_path.exists():
            logger.error(f"RoR2.h not found at {self.game_dump_path}")
            sys.exit(1)

        if not self.game_structs_path.exists():
            logger.error(f"GameStructs.h not found at {self.game_structs_path}")
            sys.exit(1)

        logger.info(f"Loading structs from {self.game_dump_path}")
        # Normalize Vector3_Value to Vector3 when loading from dump
        dump_structs = self.parser.parse_file(self.game_dump_path, normalize_vector_types=True)

        logger.info(f"Loading structs from {self.game_structs_path}")
        game_structs = self.parser.parse_file(self.game_structs_path)

        return dump_structs, game_structs

    def find_corresponding_struct(self, game_struct: Struct, dump_structs: Dict[str, Struct]) -> Optional[Struct]:
        """Find the corresponding struct in the dump file."""
        # For _Value structs, look for the base name without _Value
        if game_struct.is_value_struct:
            base_name = game_struct.name[:-6]  # Remove "_Value"
            return dump_structs.get(base_name)
        else:
            return dump_structs.get(game_struct.name)

    def compare_structs(self, game_struct: Struct, dump_struct: Struct) -> bool:
        """Compare two structs to see if they match."""
        game_normalized = game_struct.normalize_for_comparison()

        # For _Value structs comparing with non-_Value structs
        if game_struct.is_value_struct and not dump_struct.is_value_struct:
            # Create a normalized version of dump_struct without first padding
            dump_members = []
            for i, (member_type, member_name, comment) in enumerate(dump_struct.members):
                if i == 0 and "padding" in member_name:
                    continue
                dump_members.append((member_type, member_name))
        else:
            dump_members = dump_struct.normalize_for_comparison()

        return game_normalized == dump_members

    def update_struct(self, game_struct: Struct, dump_struct: Struct) -> Struct:
        """Update a game struct based on the dump struct."""
        # Copy members from dump struct
        new_members = []

        if game_struct.is_value_struct and not dump_struct.is_value_struct:
            # Special handling for _Value structs: skip first padding and remove offset comments
            for i, (member_type, member_name, comment) in enumerate(dump_struct.members):
                if i == 0 and "padding" in member_name:
                    continue
                # Don't include offset comments for _Value structs
                new_members.append((member_type, member_name, ""))
        else:
            # For regular structs (including non-_Value), copy all members as-is with all padding
            new_members = dump_struct.members.copy()

        # Update generation comment
        new_generation_comment = f"// Generated from {dump_struct.generation_comment.replace('// Generated from ', '') if dump_struct.generation_comment else dump_struct.name}"

        return Struct(game_struct.name, new_members, new_generation_comment)

    def update_file(self, dry_run: bool = False):
        """Update the GameStructs.h file."""
        dump_structs, game_structs = self.load_structs()

        updates_needed = []
        warnings = []

        for struct_name, game_struct in game_structs.items():
            # Check for generation comment
            if not game_struct.generation_comment:
                warnings.append(f"Struct {struct_name} missing generation comment, skipping")
                continue

            # Find corresponding struct in dump
            dump_struct = self.find_corresponding_struct(game_struct, dump_structs)
            if not dump_struct:
                warnings.append(f"Struct {struct_name} not found in RoR2.h dump")
                continue

            # Compare structs
            if not self.compare_structs(game_struct, dump_struct):
                logger.info(f"Struct {struct_name} needs update")
                updated_struct = self.update_struct(game_struct, dump_struct)
                updates_needed.append((struct_name, game_struct, updated_struct))

        # Print warnings
        for warning in warnings:
            logger.warning(warning)

        if not updates_needed:
            logger.info("All structs are up to date!")
            return

        logger.info(f"Found {len(updates_needed)} struct(s) that need updating")

        if dry_run:
            logger.info("Dry run mode - showing changes that would be made:")
            for struct_name, old_struct, new_struct in updates_needed:
                print(f"\n{Colors.bold(Colors.cyan(f'=== {struct_name} ==='))}")

                # Create a unified diff
                old_lines = str(old_struct).splitlines(keepends=True)
                new_lines = str(new_struct).splitlines(keepends=True)

                diff = difflib.unified_diff(
                    old_lines,
                    new_lines,
                    fromfile=f"{struct_name} (current)",
                    tofile=f"{struct_name} (updated)",
                    lineterm='',
                    n=0  # No context lines
                )

                # Print colored diff
                for line in diff:
                    line = line.rstrip()
                    if line.startswith('---'):
                        print(Colors.red(line))
                    elif line.startswith('+++'):
                        print(Colors.green(line))
                    elif line.startswith('@@'):
                        print(Colors.cyan(line))
                    elif line.startswith('-'):
                        print(Colors.red(line))
                    elif line.startswith('+'):
                        print(Colors.green(line))
                    else:
                        print(line)

                # Also show side-by-side comparison
                print(f"\n{Colors.yellow('Side-by-side comparison:')}")
                print(f"{Colors.red('- Removed')} | {Colors.green('+ Added')}")
                print("-" * 80)

                # Create word-level diff for each line
                for old_line, new_line in zip(old_lines, new_lines):
                    old_line = old_line.rstrip()
                    new_line = new_line.rstrip()
                    if old_line != new_line:
                        # Highlight differences within the line
                        matcher = difflib.SequenceMatcher(None, old_line, new_line)
                        print(f"{Colors.red('- ' + old_line)}")
                        print(f"{Colors.green('+ ' + new_line)}")
            return

        # Read the entire file
        with open(self.game_structs_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # Replace structs by finding them in the original content
        for struct_name, old_struct, new_struct in updates_needed:
            # Find the struct in the original content using the same parsing logic
            struct_start_pattern = re.compile(rf'^struct\s+{re.escape(struct_name)}\s*\{{', re.MULTILINE)
            match = struct_start_pattern.search(content)

            if not match:
                logger.warning(f"Could not find struct {struct_name} definition in file")
                continue

            # Find the generation comment before this struct
            struct_start_pos = match.start()
            comment_start_pos = struct_start_pos

            # Look backwards for the generation comment
            lines_before = content[:struct_start_pos].split('\n')
            for i, line in enumerate(reversed(lines_before[-10:])):
                line = line.strip()
                if line.startswith('// Generated from'):
                    # Found the comment, calculate its position
                    comment_line_index = len(lines_before) - 1 - i
                    comment_start_pos = len('\n'.join(lines_before[:comment_line_index])) + (1 if comment_line_index > 0 else 0)
                    break
                elif line and not line.startswith('//'):
                    break

            # Find the end of the struct (closing brace)
            brace_count = 0
            pos = match.end()

            while pos < len(content):
                if content[pos] == '{':
                    brace_count += 1
                elif content[pos] == '}':
                    if brace_count == 0:
                        # Found the closing brace
                        struct_end_pos = pos + 1
                        # Include the semicolon if present
                        if pos + 1 < len(content) and content[pos + 1] == ';':
                            struct_end_pos = pos + 2
                        break
                    brace_count -= 1
                pos += 1
            else:
                logger.warning(f"Could not find closing brace for struct {struct_name}")
                continue

            # Replace the struct
            old_content = content[comment_start_pos:struct_end_pos]
            new_content = str(new_struct)

            content = content[:comment_start_pos] + new_content + content[struct_end_pos:]
            logger.info(f"Updated struct {struct_name}")

        # Write back
        with open(self.game_structs_path, 'w', encoding='utf-8') as f:
            f.write(content)

        logger.info(f"Successfully updated {len(updates_needed)} struct(s)")

def main():
    parser = argparse.ArgumentParser(description="Update GameStructs.h based on RoR2.h dump")
    parser.add_argument(
        "--game-dump-path",
        type=Path,
        default=Path.home() / ".steam/debian-installation/steamapps/common/Risk of Rain 2/gameDump",
        help="Path to the gameDump directory containing RoR2.h"
    )
    parser.add_argument(
        "--game-structs-path",
        type=Path,
        default=Path(__file__).parent.parent / "src/game/GameStructs.h",
        help="Path to GameStructs.h file"
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Show what would be changed without actually modifying files"
    )
    parser.add_argument(
        "--no-color",
        action="store_true",
        help="Disable colored output"
    )

    args = parser.parse_args()

    # Disable colors if requested or if not in a TTY
    if args.no_color or not sys.stdout.isatty():
        Colors.RED = ''
        Colors.GREEN = ''
        Colors.YELLOW = ''
        Colors.BLUE = ''
        Colors.MAGENTA = ''
        Colors.CYAN = ''
        Colors.WHITE = ''
        Colors.RESET = ''
        Colors.BOLD = ''

    updater = StructUpdater(args.game_dump_path, args.game_structs_path)
    updater.update_file(args.dry_run)

if __name__ == "__main__":
    main()