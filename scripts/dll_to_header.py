#!/usr/bin/env python3
"""
Convert a DLL file to a C++ header file with embedded byte array.
Usage: dll_to_header.py <input.dll> <output.hpp>
"""

import sys
import os

def dll_to_header(dll_path, header_path):
    """Convert DLL file to C++ header with byte array and string offsets"""

    if not os.path.exists(dll_path):
        print(f"Error: DLL file not found: {dll_path}")
        return False

    try:
        with open(dll_path, 'rb') as f:
            dll_bytes = f.read()

        search_string = b"RoR2ModHelper"
        offsets = []
        start = 0
        while True:
            pos = dll_bytes.find(search_string, start)
            if pos == -1:
                break
            offsets.append(pos)
            start = pos + 1

        print(f"Found {len(offsets)} occurrences of 'RoR2ModHelper' at positions: {offsets}")

        # Generate header content
        header_content = [
            "// Auto-generated from RoR2ModHelper.dll",
            "// DO NOT EDIT - This file is generated during build",
            f"// Generated from: {os.path.basename(dll_path)}",
            f"// Size: {len(dll_bytes)} bytes",
            "",
            f"static const size_t RoR2ModHelper_dll_size = {len(dll_bytes)};",
            "",
            "// Offsets where 'RoR2ModHelper' string appears in the bytecode",
            f"static const size_t RoR2ModHelper_string_offsets[] = {{",
        ]

        if offsets:
            offset_strs = [str(offset) for offset in offsets]
            header_content.append(f"    {', '.join(offset_strs)}")
        header_content.extend([
            "};",
            f"static const size_t RoR2ModHelper_string_offset_count = {len(offsets)};",
            f"static const size_t RoR2ModHelper_string_length = {len(search_string)};",
            "",
            "static const unsigned char RoR2ModHelper_dll_bytes[] = {",
        ])

        # Add bytes in rows of 16 for readability
        for i in range(0, len(dll_bytes), 16):
            chunk = dll_bytes[i:i+16]
            hex_values = [f"0x{byte:02X}" for byte in chunk]
            line = "    " + ", ".join(hex_values)

            if i + 16 < len(dll_bytes):
                line += ","

            header_content.append(line)

        header_content.extend([
            "};",
            ""
        ])

        # Write header file
        os.makedirs(os.path.dirname(header_path), exist_ok=True)
        with open(header_path, 'w') as f:
            f.write('\n'.join(header_content))

        print(f"Successfully converted {dll_path} to {header_path}")
        print(f"Generated header with {len(dll_bytes)} bytes")
        return True

    except Exception as e:
        print(f"Error converting DLL to header: {e}")
        return False

def main():
    if len(sys.argv) != 3:
        print("Usage: dll_to_header.py <input.dll> <output.hpp>")
        sys.exit(1)

    dll_path = sys.argv[1]
    header_path = sys.argv[2]

    if not dll_to_header(dll_path, header_path):
        sys.exit(1)

if __name__ == "__main__":
    main()
