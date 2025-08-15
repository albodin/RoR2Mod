#!/usr/bin/env python3
"""
Extract all InteractableSpawnCard paths from Risk of Rain 2 using UnityPy.
This script parses Unity asset files to find all spawn card references.
"""

import os
import sys
import json
from pathlib import Path
from typing import List, Set, Dict, Any, Tuple
import argparse

try:
    import UnityPy
except ImportError:
    print("Error: UnityPy not installed. Run: pip install UnityPy")
    sys.exit(1)

class RoR2SpawnCardExtractor:
    def __init__(self, game_dir: Path):
        self.game_dir = game_dir
        self.spawn_card_paths: Set[str] = set()
        self.spawn_card_data: Dict[str, Any] = {}

    def find_asset_files(self) -> List[Path]:
        """Find all Unity asset files in the game directory."""
        asset_files = []

        data_dir = self.game_dir / "Risk of Rain 2_Data"
        if data_dir.exists():
            for pattern in ["*.assets", "*.resource", "*.bundle", "*.resS"]:
                asset_files.extend(data_dir.glob(f"**/{pattern}"))

        print(f"Found {len(asset_files)} asset files to scan")
        return asset_files

    def extract_spawn_cards_from_file(self, asset_path: Path) -> Tuple[Set[str], Dict[str, Any], int]:
        """Extract spawn card information from a Unity asset file."""
        local_paths = set()
        local_data = {}
        found_in_file = 0

        try:
            env = UnityPy.load(str(asset_path))

            for obj in env.objects:
                try:
                    data = obj.read()

                    if hasattr(data, 'm_Name') and data.m_Name:
                        name = data.m_Name

                        if name.startswith('isc'):
                            path = f"SpawnCards/InteractableSpawnCard/{name}"
                            local_paths.add(path)

                            try:
                                tree_data = obj.read_typetree()
                                local_data[name] = {
                                    'path': path,
                                    'file': asset_path.name,
                                    'path_id': obj.path_id,
                                    'type': obj.type.name,
                                    'data': tree_data
                                }
                                found_in_file += 1
                            except:
                                local_data[name] = {
                                    'path': path,
                                    'file': asset_path.name,
                                    'path_id': obj.path_id,
                                    'type': obj.type.name,
                                    'name': name
                                }
                                found_in_file += 1

                except Exception as e:
                    continue

        except Exception as e:
            print(f"Error processing {asset_path}: {e}")

        return local_paths, local_data, found_in_file

    def extract_all_spawn_cards(self) -> None:
        """Extract spawn cards from all asset files."""
        asset_files = self.find_asset_files()

        for i, asset_file in enumerate(asset_files, 1):
            local_paths, local_data, found_count = self.extract_spawn_cards_from_file(asset_file)

            self.spawn_card_paths.update(local_paths)
            self.spawn_card_data.update(local_data)

            if found_count > 0:
                print(f"[{i}/{len(asset_files)}] {asset_file.name} → Found {found_count} spawn cards")
            else:
                print(f"[{i}/{len(asset_files)}] {asset_file.name}")

        print(f"\nTotal unique spawn cards found: {len(self.spawn_card_paths)}")

    def generate_header_file(self, output_path: Path) -> None:
        """Generate C++ header file with all discovered paths."""
        sorted_paths = sorted(self.spawn_card_paths)

        header_content = f'''#pragma once
// Auto-generated InteractableSpawnCard paths from Unity assets
// Extracted {len(self.spawn_card_data)} spawn cards using UnityPy
// Generated from {self.game_dir.name}

#include <string>
#include <vector>

namespace InteractablePaths {{
const std::vector<std::string> ALL_INTERACTABLE_PATHS = {{
'''

        for path in sorted_paths:
            name = path.split('/')[-1]
            header_content += f'    "{path}",\n'

        header_content += f'''}};
}} // namespace InteractablePaths
'''

        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w') as f:
            f.write(header_content)

        print(f"Generated header file: {output_path}")
        print(f"Total paths: {len(sorted_paths)}")

    def save_detailed_report(self, output_path: Path) -> None:
        """Save detailed JSON report of all findings."""
        report = {
            "extraction_info": {
                "game_directory": str(self.game_dir),
                "total_spawn_cards": len(self.spawn_card_paths),
                "paths_found": len(self.spawn_card_paths)
            },
            "spawn_card_paths": sorted(list(self.spawn_card_paths)),
            "detailed_data": self.spawn_card_data
        }

        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w') as f:
            json.dump(report, f, indent=2)

        print(f"Saved detailed report: {output_path}")

    def print_summary(self) -> None:
        """Print a summary of findings."""
        print(f"\n=== EXTRACTION SUMMARY ===")
        print(f"Game directory: {self.game_dir}")
        print(f"Total spawn cards found: {len(self.spawn_card_paths)}")

        print("\nFound spawn cards:")
        for path in sorted(self.spawn_card_paths):
            name = path.split('/')[-1]
            print(f"  - {name}")

def main():
    parser = argparse.ArgumentParser(description="Extract InteractableSpawnCard paths from Risk of Rain 2")
    default_paths = [
        Path.home() / ".local/share/Steam/steamapps/common/Risk of Rain 2",
        Path.home() / ".steam/debian-installation/steamapps/common/Risk of Rain 2"
    ]
    default_path = None
    for path in default_paths:
        if path.exists():
            default_path = path
            break

    if not default_path:
        default_path = default_paths[0]

    parser.add_argument(
        "--game-dir",
        type=Path,
        default=default_path,
        help="Path to Risk of Rain 2 installation"
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path(__file__).parent.parent / "src/game",
        help="Output directory for generated files"
    )
    parser.add_argument(
        "--detailed-report",
        action="store_true",
        help="Generate detailed JSON report (saved to scripts directory)"
    )

    args = parser.parse_args()

    if not args.game_dir.exists():
        print(f"Error: Game directory not found: {args.game_dir}")
        sys.exit(1)

    print(f"Extracting spawn cards from: {args.game_dir}")

    extractor = RoR2SpawnCardExtractor(args.game_dir)
    extractor.extract_all_spawn_cards()

    extractor.generate_header_file(args.output_dir / "InteractablePaths.h")

    if args.detailed_report:
        script_dir = Path(__file__).parent
        extractor.save_detailed_report(script_dir / "spawn_card_extraction_report.json")

    extractor.print_summary()

if __name__ == "__main__":
    main()
