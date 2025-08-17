#!/usr/bin/env python3
"""
Extract InteractableSpawnCard addressable paths from Risk of Rain 2's lrapi_returns.json.
This script reads the addressable asset mappings to get the exact paths used by the game.
"""

import sys
import json
from pathlib import Path
from typing import Dict
import argparse

ISC_DISPLAY_NAMES = {
    "iscBarrel1": "Barrel",
    "iscBrokenDrone1": "Broken Gunner Drone",
    "iscBrokenDrone2": "Broken Healing Drone",
    "iscBrokenEmergencyDrone": "Broken Emergency Drone",
    "iscBrokenEquipmentDrone": "Broken Equipment Drone",
    "iscBrokenFlameDrone": "Broken Incinerator Drone",
    "iscBrokenMegaDrone": "Broken TC-280",
    "iscBrokenMissileDrone": "Broken Missile Drone",
    "iscBrokenTurret1": "Broken Gunner Turret",
    "iscCasinoChest": "Adaptive Chest",
    "iscCategoryChest2Damage": "Large Chest - Damage",
    "iscCategoryChest2Healing": "Large Chest - Healing",
    "iscCategoryChest2Utility": "Large Chest - Utility",
    "iscCategoryChestDamage": "Chest - Damage",
    "iscCategoryChestHealing": "Chest - Healing",
    "iscCategoryChestUtility": "Chest - Utility",
    "iscChest1": "Chest",
    "iscChest1Stealthed": "Cloaked Chest",
    "iscChest2": "Large Chest",
    "iscColossusPortal": "Green Portal",
    "iscCommandChest": "Command Chest",
    "iscDeepVoidPortal": "Deep Void Portal",
    "iscDeepVoidPortalBattery": "Deep Void Signal",
    "iscDestinationPortal": "Destination Portal",
    "iscDuplicator": "3D Printer",
    "iscDuplicatorLarge": "3D Printer (Green)",
    "iscDuplicatorMilitary": "Mili-Tech Printer",
    "iscDuplicatorWild": "Overgrown 3D Printer",
    "iscEquipmentBarrel": "Equipment Barrel",
    "iscEventTrigger": "Prime Meridian - Event Trigger",
    "iscFreeChest": "Crashed Multishop Terminal",
    "iscGauntletEntrance": "Gauntlet Entrance",
    "iscGeode": "Aurelionite Geode",
    "iscGoldChest": "Legendary Chest",
    "iscGoldshoresBeacon": "Halcyon Beacon",
    "iscGoldshoresPortal": "Gold Portal",
    "iscInfiniteTowerPortal": "Infinite Portal",
    "iscInfiniteTowerSafeWard": "Assessment Focus",
    "iscInfiniteTowerSafeWardAwaitingInteraction": "Assessment Focus (Awaiting)",
    "iscLemurianEgg": "Lemurian Egg",
    "iscLockbox": "Rusty Lockbox",
    "iscLockboxVoid": "Encrusted Lockbox",
    "iscLunarChest": "Lunar Pod",
    "iscLunarTeleporter": "Teleporter (Lunar)",
    "iscMSPortal": "Celestial Portal",
    "iscRadarTower": "Radio Scanner",
    "iscScavBackpack": "Scavenger's Sack",
    "iscScavLunarBackpack": "Scavenger Backpack (Lunar)",
    "iscScrapper": "Scrapper",
    "iscShopPortal": "Blue Portal",
    "iscShrineBlood": "Shrine of Blood",
    "iscShrineBloodSandy": "Shrine of Blood (Sandy)",
    "iscShrineBloodSnowy": "Shrine of Blood (Snowy)",
    "iscShrineBoss": "Shrine of the Mountain",
    "iscShrineBossSandy": "Shrine of the Mountain (Sandy)",
    "iscShrineBossSnowy": "Shrine of the Mountain (Snowy)",
    "iscShrineChance": "Shrine of Chance",
    "iscShrineChanceSandy": "Shrine of Chance (Sandy)",
    "iscShrineChanceSnowy": "Shrine of Chance (Snowy)",
    "iscShrineCleanse": "Cleansing Pool",
    "iscShrineCleanseSandy": "Cleansing Pool (Sandy)",
    "iscShrineCleanseSnowy": "Cleansing Pool (Snowy)",
    "iscShrineColossusAccess": "Shrine of Shaping",
    "iscShrineCombat": "Shrine of Combat",
    "iscShrineCombatSandy": "Shrine of Combat (Sandy)",
    "iscShrineCombatSnowy": "Shrine of Combat (Snowy)",
    "iscShrineGoldshoresAccess": "Altar of Gold",
    "iscShrineHalcyonite": "Halcyon Shrine",
    "iscShrineHalcyoniteTier1": "Halcyon Shrine Tier 1",
    "iscShrineHealing": "Shrine of the Woods",
    "iscShrineRestack": "Shrine of Order",
    "iscShrineRestackSandy": "Shrine of Order (Sandy)",
    "iscShrineRestackSnowy": "Shrine of Order (Snowy)",
    "iscSquidTurret": "Squid Turret",
    "iscTeleporter": "Teleporter",
    "iscTripleShop": "Multishop Terminal",
    "iscTripleShopEquipment": "Multishop Terminal (Equipment)",
    "iscTripleShopLarge": "Multishop Terminal (Green)",
    "iscVoidCamp": "Void Seed",
    "iscVoidChest": "Void Cradle",
    "iscVoidChestSacrificeOn": "Void Cradle (Sacrifice)",
    "iscVoidCoinBarrel": "Stalk",
    "iscVoidOutroPortal": "Void Outro Portal",
    "iscVoidPortal": "Void Portal",
    "iscVoidRaidSafeWard": "Cell Vent",
    "iscVoidSuppressor": "Void Eradicator",
    "iscVoidTriple": "Void Potential",
}

class RoR2SpawnCardExtractor:
    def __init__(self, game_dir: Path):
        self.game_dir = game_dir
        self.addressable_paths: Dict[str, str] = {}

    def extract_addressable_paths_from_lrapi(self) -> None:
        """Extract addressable paths from lrapi_returns.json."""
        lrapi_path = self.game_dir / "Risk of Rain 2_Data" / "StreamingAssets" / "lrapi_returns.json"

        if lrapi_path.exists():
            print(f"Found lrapi_returns.json, extracting addressable paths...")
            try:
                with open(lrapi_path, 'r') as f:
                    lrapi_data = json.load(f)

                # Extract all ISC entries (InteractableSpawnCard assets)
                for key in lrapi_data.keys():
                    if '/isc' in key and key.endswith('.asset'):
                        # Extract spawn card name from path (e.g., "iscChest1" from "RoR2/Base/Chest1/iscChest1.asset")
                        name = key.split('/')[-1].replace('.asset', '')
                        self.addressable_paths[name] = key

                print(f"Extracted {len(self.addressable_paths)} addressable paths from lrapi_returns.json")
            except Exception as e:
                print(f"Error parsing lrapi_returns.json: {e}")
        else:
            print(f"lrapi_returns.json not found at {lrapi_path}")

    def extract_all_spawn_cards(self) -> None:
        """Extract all InteractableSpawnCard addressable paths."""
        self.extract_addressable_paths_from_lrapi()

    def generate_header_file(self, output_path: Path) -> None:
        """Generate C++ header file with addressable paths."""
        header_content = f'''#pragma once
// Auto-generated InteractableSpawnCard addressable paths from lrapi_returns.json
// Extracted {len(self.addressable_paths)} addressable paths
// Generated from {self.game_dir.name}

#include <string>
#include <unordered_map>

namespace InteractablePaths {{

const std::unordered_map<std::string, std::string> INTERACTABLES = {{
'''

        entries = []
        for name, addr_path in self.addressable_paths.items():
            display_name = ISC_DISPLAY_NAMES.get(name, name)
            entries.append((display_name, addr_path))
        for display_name, addr_path in sorted(entries):
            header_content += f'    {{"{display_name}", "{addr_path}"}},\n'

        header_content += f'''}};

}} // namespace InteractablePaths
'''

        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w') as f:
            f.write(header_content)

        print(f"Generated header file: {output_path}")

    def save_detailed_report(self, output_path: Path) -> None:
        """Save detailed JSON report of all findings."""
        report = {
            "extraction_info": {
                "game_directory": str(self.game_dir),
                "addressable_paths_found": len(self.addressable_paths)
            },
            "addressable_paths": self.addressable_paths
        }

        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w') as f:
            json.dump(report, f, indent=2)

        print(f"Saved detailed report: {output_path}")

    def print_summary(self) -> None:
        """Print a summary of findings."""
        print(f"\n=== EXTRACTION SUMMARY ===")
        print(f"Game directory: {self.game_dir}")
        print(f"Total addressable paths found: {len(self.addressable_paths)}")

        mapped_display_names = len([name for name in self.addressable_paths if name in ISC_DISPLAY_NAMES])
        print(f"ISCs with display names: {mapped_display_names}/{len(self.addressable_paths)}")

        if self.addressable_paths:
            print("\nAddressable paths:")
            for name, path in sorted(self.addressable_paths.items()):
                display_name = ISC_DISPLAY_NAMES.get(name, name)
                comment = f" // {display_name}" if display_name != name else ""
                print(f"  - {name}: {path}{comment}")

def find_game_directory() -> Path:
    """Find the Risk of Rain 2 installation directory."""
    possible_paths = [
        Path.home() / ".local/share/Steam/steamapps/common/Risk of Rain 2",
        Path.home() / ".steam/debian-installation/steamapps/common/Risk of Rain 2"
    ]

    for path in possible_paths:
        if path.exists():
            return path

    return possible_paths[0]

def main():
    parser = argparse.ArgumentParser(description="Extract InteractableSpawnCard paths from Risk of Rain 2")
    default_path = find_game_directory()

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
