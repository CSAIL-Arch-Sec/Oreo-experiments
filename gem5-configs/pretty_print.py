from enum import Enum

# https://svn.blender.org/svnroot/bf-blender/trunk/blender/build_files/scons/tools/bcolors.py
class bcolors:
    HEADER = '\033[95m'  # magenta ?
    OKBLUE = '\033[94m'  # purple ?
    OKCYAN = '\033[96m'  # cyan
    OKGREEN = '\033[92m' # green
    WARNING = '\033[93m' # yellow
    FAIL = '\033[91m'    # red
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    NONE = ''

class MessageType(Enum):
    DEFAULT = "[*]", bcolors.NONE, False, False
    MAGENTA = "[*]", bcolors.HEADER, False, True
    PURPLE = "[*]", bcolors.OKBLUE, False, True
    CYAN = "[*]", bcolors.OKCYAN, False, True
    GREEN = "[*]", bcolors.OKGREEN, False, True
    RED = "[*]", bcolors.WARNING, False, True
    MAGENTA_ = "[*]", bcolors.HEADER, False, False
    PURPLE_ = "[*]", bcolors.OKBLUE, False, False
    CYAN_ = "[*]", bcolors.OKCYAN, False, False
    GREEN_ = "[*]", bcolors.OKGREEN, False, False
    RED_ = "[*]", bcolors.WARNING, False, False
    YELLOW_ = "[*]", bcolors.FAIL, False, False
    WARNING = "[WARN]", bcolors.WARNING, False, True
    FAIL = "[FAIL]", bcolors.FAIL, False, True
    CHECKPOINT = "[CHCKPT]", bcolors.OKCYAN, False, True
    CONFIG = "[CONFIG]", bcolors.OKBLUE, False, False
    
def pretty_print(message: str, message_type: MessageType = MessageType.DEFAULT):
    lead_string, bcolor, is_bold, is_color = message_type.value
    endc = bcolors.ENDC
    bbold = bcolors.BOLD
    lead_string = bbold + bcolor + lead_string + endc + endc
    body_string = message
    if is_bold:
        body_string = bbold + body_string + endc
    if is_color:
        body_string = bcolor + body_string + endc
    print(f'{lead_string} {body_string}')

def test_pretty_print(message: str):
    for message_type in MessageType:
        pretty_print(message, message_type)