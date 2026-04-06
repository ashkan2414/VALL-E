import os, glob, re

def process_enum(match):
    enum_name = match.group(1)
    base_type_match = match.group(2).strip()
    body = match.group(3)

    lines = body.split('\n')
    items = []
    
    for line in lines:
        clean_line = line.split('//')[0].split('/*')[0].strip()
        if not clean_line: continue
        
        parts = clean_line.split('=')
        key = parts[0].strip()
        if key.endswith(','): key = key[:-1].strip()
        
        value = parts[1].strip() if len(parts) > 1 else ''
        if value.endswith(','): value = value[:-1].strip()
        
        if key:
            items.append((key, value))

    if not items:
        return match.group(0)

    # analyze values
    all_numbers = True
    all_ll = True
    for k, v in items:
        if not re.match(r'^-?\d+$', v):
            all_numbers = False
        if not v.startswith('LL_') and not v.startswith('UART_') and not v.startswith('USART_') and not v.startswith('HAL_'):
            all_ll = False

    indent = "        "
    
    if all_numbers:
        base = f"EnumValue<{enum_name}, uint32_t>"
        decl = f"    struct {enum_name} : public {base}\n    {{\n"
        for k, v in items:
            decl += f"{indent}static const {enum_name} {k};\n"
        decl += "    };\n\n"
        for k, v in items:
            decl += f"    inline constexpr {enum_name} {enum_name}::{k} = {enum_name}::from_number<{v}>();\n"
        return decl

    else:
        # Default to LLDriverEnumValue
        base = f"LLDriverEnumValue<{enum_name}>"
        decl = f"    struct {enum_name} : public {base}\n    {{\n"
        for k, v in items:
            decl += f"{indent}static const {enum_name} {k};\n"
        decl += "    };\n\n"
        for k, v in items:
            val = v if v else '0' # careful
            decl += f"    inline constexpr {enum_name} {enum_name}::{k} = {enum_name}::from_ll<{val}>();\n"
        return decl

def modify_file(filepath):
    print("Modifying " + filepath)
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    new_content = re.sub(r'    enum\s+class\s+(\w+)(.*?)\{([^{}]+)\};', process_enum, content, flags=re.DOTALL)
    
    if new_content != content:
        with open(filepath, 'w', encoding='utf-8', newline='\n') as f:
            f.write(new_content)

files = glob.glob('C:/Users/ebrah/Documents/Documents/Source/Capstone/VALL-E/firmware/src/platform/stm32/g474xx/inc/valle/platform/hardware/**/interface.hpp', recursive=True)
for f in files:
    modify_file(f)
