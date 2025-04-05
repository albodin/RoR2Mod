#!/usr/bin/env python3
import subprocess
import os
import gdb

class LoadDllSymbols(gdb.Command):
    """Command to load DLL symbols with correct section addresses.
    Usage: load-dll PATH_TO_DLL"""
    
    def __init__(self):
        super(LoadDllSymbols, self).__init__("load-dll", gdb.COMMAND_FILES)
        self.script_dir = os.path.dirname(os.path.abspath(__file__))
        self.project_root = os.path.dirname(self.script_dir)
        self.build_dir = os.path.join(self.project_root, "build")

        print(f"Script directory: {self.script_dir}")
        print(f"Project root: {self.project_root}")
        print(f"Build directory: {self.build_dir}")
    
    def invoke(self, arg, from_tty):
        args = gdb.string_to_argv(arg)
        if len(args) == 0:
            dll_path = os.path.join(self.build_dir, "RoR2Mod.dll")
        else:
            dll_path = args[0]
        if not os.path.isfile(dll_path):
            print(f"Error: File not found: {dll_path}")
            return
        
        try:
            self.add_symbols_to_gdb(dll_path)
        except Exception as e:
            print(f"Error: {str(e)}")
            import traceback
            traceback.print_exc()
    
    def parse_objdump(self, dll_path):
        """Parse the output of objdump -h to get section information."""
        result = subprocess.run(['objdump', '-h', dll_path], 
                               capture_output=True, text=True, check=True)
        
        sections = {}
        in_sections = False
        
        for line in result.stdout.splitlines():
            if "Sections:" in line:
                in_sections = True
                continue
                
            if not in_sections:
                continue
                
            # Skip header line
            if "Idx Name" in line:
                continue
            
            # Process section entries
            parts = line.strip().split()
            if len(parts) >= 6 and parts[0].isdigit() and parts[1].startswith('.'):
                try:
                    name = parts[1]
                    size = int(parts[2], 16) if '0x' not in parts[2] else int(parts[2][2:], 16)
                    vma = int(parts[3], 16) if '0x' not in parts[3] else int(parts[3][2:], 16)
                    
                    sections[name] = {
                        'size': size,
                        'vma': vma
                    }
                except (ValueError, IndexError):
                    continue
        
        return sections
    
    def find_dll_mappings(self, dll_path):
        """Find all memory mappings related to the DLL."""
        result = gdb.execute("info proc mappings", to_string=True)
        
        dll_basename = os.path.basename(dll_path)
        mappings = []
        dll_found = False
        
        for line in result.splitlines():
            if "Start Addr" in line:  # Skip header
                continue
                
            parts = line.strip().split()
            if len(parts) < 5:  # Need at least addresses, size, offset, permissions
                continue
                
            try:
                mapping = {
                    'start_addr': int(parts[0], 16),
                    'end_addr': int(parts[1], 16),
                    'size': int(parts[2], 16),
                    'offset': int(parts[3], 16),
                    'permissions': parts[4],
                    'file': " ".join(parts[5:]) if len(parts) > 5 else ""
                }
                
                # Check if this is our DLL
                if dll_path in mapping['file'] or dll_basename in mapping['file']:
                    mappings.append(mapping)
                    dll_found = True
                elif dll_found and mapping['file'] == "":
                    # Include adjacent mappings with no filename (likely part of the same DLL)
                    mappings.append(mapping)
            except (ValueError, IndexError):
                continue
        
        return mappings
    
    def find_text_section_addr(self, mappings):
        """Find the address of the .text section in memory."""
        # First try to find an executable mapping
        for mapping in mappings:
            if 'x' in mapping['permissions']:
                return mapping['start_addr']
        
        # If no executable mapping found, use the first mapping
        if mappings:
            return mappings[0]['start_addr']
        
        return None
    
    def add_symbols_to_gdb(self, dll_path):
        """Load symbols into GDB with correct section addresses."""
        print(f"Processing DLL: {dll_path}")
        
        # Get section info from objdump
        sections = self.parse_objdump(dll_path)
        if not sections or '.text' not in sections:
            raise Exception("Failed to find .text section in the DLL")
        
        text_vma = sections['.text']['vma']
        print(f"Found .text section at VMA: 0x{text_vma:x}")
        
        # Get memory mappings
        mappings = self.find_dll_mappings(dll_path)
        if not mappings:
            raise Exception(f"Could not find memory mappings for {os.path.basename(dll_path)}")
        
        # Find the .text section in memory
        text_addr = self.find_text_section_addr(mappings)
        if text_addr is None:
            raise Exception("Could not determine memory address for .text section")
        
        print(f".text section loaded at: 0x{text_addr:x}")
        
        # Calculate the offset between file VMA and memory address
        vma_offset = text_addr - text_vma
        
        # Build the add-symbol-file command
        cmd = f"add-symbol-file {dll_path} 0x{text_addr:x}"
        
        for section_name in sections:
            if section_name == '.text':
                continue
            section_addr = sections[section_name]['vma'] + vma_offset
            cmd += f" -s {section_name} 0x{section_addr:x}"
            print(f"{section_name} section at: 0x{section_addr:x}")
        
        # Execute the command
        print(f"Executing: {cmd}")
        gdb.execute(cmd)
        print(f"Symbols loaded successfully for {os.path.basename(dll_path)}")

# Register the command
LoadDllSymbols()
print("'load-dll' command has been loaded. Use: load-dll PATH_TO_DLL")