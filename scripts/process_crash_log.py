#!/usr/bin/env python3
import argparse
import re
import subprocess
import os
import glob

def parse_arguments():
    parser = argparse.ArgumentParser(description='Process RoR2 crash logs and map to source code')
    parser.add_argument('log_file', nargs='?', help='Path to the game log file (optional, will find latest if not specified)')
    parser.add_argument('--dll', '-d', help='Path to the RoR2Mod.dll binary (default: build/RoR2Mod.dll)',
                        default='build/RoR2Mod.dll')
    return parser.parse_args()

def find_latest_crash_log():
    possible_paths = [
        os.path.expanduser("~/.steam/debian-installation/steamapps/compatdata/632360/pfx/drive_c/users/steamuser/AppData/Local/Temp/Hopoo Games, LLC/Risk of Rain 2/Crashes/"),
        os.path.expanduser("~/.local/share/Steam/steamapps/compatdata/632360/pfx/drive_c/users/steamuser/AppData/Local/Temp/Hopoo Games, LLC/Risk of Rain 2/Crashes/")
    ]
    
    base_path = None
    for path in possible_paths:
        if os.path.exists(path):
            base_path = path
            print(f"Using crash directory: {base_path}")
            break
    
    if base_path is None:
        print("Crash directory not found at any of these locations:")
        for path in possible_paths:
            print(f"  {path}")
        return None

    crash_dirs = glob.glob(os.path.join(base_path, "Crash_*"))

    if not crash_dirs:
        print("No crash directories found")
        return None

    latest_dir = max(crash_dirs, key=os.path.getmtime)
    log_file = os.path.join(latest_dir, "Player.log")

    if not os.path.exists(log_file):
        print(f"Player.log not found in latest crash directory: {latest_dir}")
        return None

    print(f"Found latest crash log: {log_file}")
    return log_file

def get_image_base(dll_path):
    """Get the ImageBase from objdump -p"""
    try:
        cmd = ['objdump', '-p', dll_path]
        result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                               universal_newlines=True)

        if result.returncode != 0:
            print(f"Error running objdump: {result.stderr}")
            return None

        for line in result.stdout.splitlines():
            if "ImageBase" in line:
                parts = line.split()
                if len(parts) >= 2:
                    try:
                        image_base = int(parts[-1], 16)
                        return image_base
                    except ValueError:
                        print(f"Failed to parse ImageBase: {line}")
                        return None

        print("ImageBase not found in objdump output")
        return None
    except Exception as e:
        print(f"Error getting ImageBase: {e}")
        return None

def get_source_context(source_file, line_number, context_lines=3):
    """Get context lines around the specified line in the source file"""
    if not os.path.exists(source_file):
        return f"Source file not found: {source_file}"

    try:
        with open(source_file, 'r') as f:
            lines = f.readlines()

        start_line = max(0, line_number - context_lines - 1)
        end_line = min(len(lines), line_number + context_lines)

        result = []
        for i in range(start_line, end_line):
            prefix = "→ " if i == line_number - 1 else "  "
            result.append(f"{prefix}{i+1}: {lines[i].rstrip()}")

        return "\n".join(result)
    except Exception as e:
        return f"Error reading source file: {e}"

def get_relevant_disassembly(dll_path, offset, context_size=10):
    """Get disassembly around the crash point with proper function boundaries"""

    cmd = ['objdump', '-d', dll_path]
    full_output = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True, text=True)

    if full_output.returncode != 0:
        return "Error running objdump: " + full_output.stderr

    lines = full_output.stdout.splitlines()

    target_addr_str = f"{offset:x}:"
    target_addr_prefix = target_addr_str[:8]

    current_function = None
    function_lines = []
    found_function = False
    target_line_idx = -1

    for i, line in enumerate(lines):
        if "<" in line and ">:" in line:
            if found_function:
                break

            current_function = line
            function_lines = [line]
            continue

        if current_function is None:
            continue

        function_lines.append(line)

        if target_addr_prefix in line:
            found_function = True
            target_line_idx = len(function_lines) - 1

    if not found_function or target_line_idx == -1:
        return f"Couldn't find address 0x{offset:x} in disassembly"

    start_idx = max(1, target_line_idx - context_size)  # Skip function header if needed
    end_idx = min(len(function_lines), target_line_idx + context_size + 1)

    result = ["  Function: " + current_function]
    result.append("  Disassembly:")

    for i in range(start_idx, end_idx):
        prefix = "→ " if i == target_line_idx else "  "
        result.append(f"  {prefix}{function_lines[i].strip()}")

    return "\n".join(result)

def process_game_log(log_file, dll_path):
    """Process the game log file to find crash addresses"""
    try:
        if not os.path.exists(log_file):
            print(f"Error: Log file not found at {log_file}")
            return False

        image_base = get_image_base(dll_path)
        if image_base is None:
            return False

        print(f"DLL ImageBase: 0x{image_base:X}")

        with open(log_file, 'r', errors='replace') as f:
            content = f.read()

        stack_trace_start = content.find("========== OUTPUTTING STACK TRACE ==================")
        stack_trace_end = content.find("========== END OF STACKTRACE ===========")

        if stack_trace_start == -1 or stack_trace_end == -1:
            print("Could not find stack trace markers in the log file")
            return False

        stack_trace = content[stack_trace_start:stack_trace_end]

        module_base = None
        module_size = None
        dll_name = os.path.basename(dll_path)

        for line in stack_trace.splitlines():
            if dll_name in line and "size:" in line:
                parts = line.split(":", 1)[1].strip().split(",")
                if len(parts) >= 2:
                    addr_part = parts[0].strip()
                    size_part = parts[1].strip()

                    addr_match = re.search(r'\(([0-9A-Fa-f]+)\)', addr_part)
                    size_match = re.search(r'size: (\d+)', size_part)

                    if addr_match and size_match:
                        module_base = int(addr_match.group(1), 16)
                        module_size = int(size_match.group(1))
                        break

        if module_base is None:
            print("Could not find module base address in log")
            return False

        print(f"Module base address: 0x{module_base:X}, size: {module_size}")

        mod_addresses = []

        # Pattern to match lines like: 0x00006FFFF814388D (ror2mod) (function-name not available)
        pattern = re.compile(r'0x([0-9A-Fa-f]+) \((ror2mod|' + re.escape(dll_name.lower()) + r')\)')

        for line in stack_trace.splitlines():
            match = pattern.search(line)
            if match:
                addr = int(match.group(1), 16)
                mod_addresses.append(addr)

        if not mod_addresses:
            print("No addresses found for your mod in the stack trace")
            return False

        print(f"Found {len(mod_addresses)} addresses for your mod in the stack trace")

        print("\n=== Source Code Mapping ===")
        for i, addr in enumerate(mod_addresses):
            # Calculate file offset that can be used with addr2line
            # Formula: address - module_base + image_base
            offset = addr - module_base + image_base

            print(f"Frame #{i}: Address 0x{addr:X}")
            print(f"  Calculated offset: 0x{offset:X}")

            try:
                cmd = ['addr2line', '-e', dll_path, '-f', '-C', '-p', f'0x{offset:X}']
                result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                       universal_newlines=True)

                if result.returncode == 0:
                    source_info = result.stdout.strip()
                    print(f"  Source: {source_info}")

                    file_line_match = re.search(r'at (.*):(\d+)', source_info)
                    if file_line_match:
                        source_file = file_line_match.group(1)
                        line_number = int(file_line_match.group(2))

                        context = get_source_context(source_file, line_number)
                        print("\n  Source context:")
                        print(f"{context}")
                else:
                    print(f"  addr2line error: {result.stderr}")

                # Try to find nearby function with nm
                cmd = ['nm', '--demangle', '--numeric-sort', dll_path]
                nm_result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                         universal_newlines=True)

                if nm_result.returncode == 0:
                    # Find nearest function before this address
                    prev_addr = 0
                    prev_name = None

                    for line in nm_result.stdout.splitlines():
                        parts = line.split(' ', 2)
                        if len(parts) >= 3 and parts[1].lower() in ['t', 'w']:
                            try:
                                func_addr = int(parts[0], 16)
                                func_name = parts[2]

                                if func_addr <= offset and func_addr > prev_addr:
                                    prev_addr = func_addr
                                    prev_name = func_name
                            except ValueError:
                                pass

                    if prev_name:
                        func_offset = offset - prev_addr
                        print(f"  Function: {prev_name} + 0x{func_offset:X}")

                # Also try objdump for disassembly
                disasm = get_relevant_disassembly(dll_path, offset)
                if disasm:
                    print("\n" + disasm)
            except Exception as e:
                print(f"  Error processing address: {e}")

            print()

        return True
    except Exception as e:
        print(f"Error processing log: {e}")
        return False

def main():
    args = parse_arguments()
    log_file = args.log_file

    if not log_file:
        log_file = find_latest_crash_log()
        if not log_file:
            print("Could not find latest crash log automatically. Please specify a log file path.")
            return False

    process_game_log(log_file, args.dll)

if __name__ == "__main__":
    main()