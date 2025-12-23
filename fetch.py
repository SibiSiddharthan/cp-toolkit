import subprocess
import sys

CODEFORCES_REFERER = "https://codeforces.com"
ATCODER_REFERER = "https://atcoder.jp"

USER_AGENT = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)"
ACCEPT_FORMATS = "Accept: text/html,application/xhtml+xml"

if len(sys.argv) != 2:
    exit(1)

cmd = []
url = sys.argv[1]

if "codeforces" in url:
    cmd = ["curl", f"{url}/problems", "-H", USER_AGENT, "-H", ACCEPT_FORMATS, "-H", f"Referer: {CODEFORCES_REFERER}"]

if "atcoder" in url:
    cmd = ["curl", f"{url}/tasks_print", "-H", USER_AGENT, "-H", ACCEPT_FORMATS, "-H", f"Referer: {ATCODER_REFERER}"]

if len(cmd) == 0:
    exit(1)

result = subprocess.run(
    cmd,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
)

sys.stdout.buffer.write(result.stdout)
