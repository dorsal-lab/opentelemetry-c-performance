import re
import statistics
import sys
from collections import OrderedDict
from pathlib import Path

if __name__ == "__main__":
    executables_and_durations = OrderedDict()
    executable = ""
    with Path(sys.argv[1]).open() as f_in:
        for line in f_in:
            match = re.match(r"\#+ Run no \d+ executable\s*=\s*(?P<executable_and_options>.*) \#+", line)
            if match:
                executable = match.group("executable_and_options")
                continue
            if not executable:
                continue
            match = re.match(r"duration\s*=\s*(?P<duration>\d+)\s*ns", line)
            if not match:
                continue
            if executable not in executables_and_durations:
                executables_and_durations[executable] = []
            executables_and_durations[executable].append(int(match.group("duration")))
    with Path(sys.argv[2]).open("w") as f_out:
        for executable, durations in executables_and_durations.items():
            f_out.write(f"{executable}\n")
            f_out.write(f"duration = {statistics.mean(durations)}\n")
