import re
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
            math_stats_and_regex = [
                ("min", r"min\s*=\s*(?P<duration>[+-]?([0-9]*[.])?[0-9]+)\s*ns"),
                ("mean", r"mean\s*=\s*(?P<duration>[+-]?([0-9]*[.])?[0-9]+)\s*ns"),
                ("max", r"max\s*=\s*(?P<duration>[+-]?([0-9]*[.])?[0-9]+)\s*ns"),
                ("median", r"median\s*=\s*(?P<duration>[+-]?([0-9]*[.])?[0-9]+)\s*ns"),
                ("std", r"std\s*=\s*(?P<duration>[+-]?([0-9]*[.])?[0-9]+)\s*ns"),
                # ("sum", r"sum\s*=\s*(?P<duration>[+-]?([0-9]*[.])?[0-9]+)\s*ns"),
            ]
            for stat, regex in math_stats_and_regex:
                match = re.match(regex, line)
                if not match:
                    continue
                if executable not in executables_and_durations:
                    executables_and_durations[executable] = OrderedDict()
                executables_and_durations[executable][stat] = float(match.group("duration"))
            time_stats_and_regex = [
                ("real", r"real\s*(?P<m>[+-]?([0-9]*[.])?[0-9]+)m(?P<s>[+-]?([0-9]*[.])?[0-9]+)s"),
                ("user", r"user\s*(?P<m>[+-]?([0-9]*[.])?[0-9]+)m(?P<s>[+-]?([0-9]*[.])?[0-9]+)s"),
                ("sys", r"sys\s*(?P<m>[+-]?([0-9]*[.])?[0-9]+)m(?P<s>[+-]?([0-9]*[.])?[0-9]+)s"),
            ]
            for stat, regex in time_stats_and_regex:
                match = re.match(regex, line)
                if not match:
                    continue
                if executable not in executables_and_durations:
                    executables_and_durations[executable] = OrderedDict()
                duration = (float(match.group("m")) * 60 + float(match.group("s"))) * 1000000000
                executables_and_durations[executable][stat] = duration
    with Path(sys.argv[2]).open("w") as f_out:
        for executable, stats in executables_and_durations.items():
            f_out.write(f"{executable}\n")
            for stat, val in stats.items():
                f_out.write(f"{stat} = {val}\n")
