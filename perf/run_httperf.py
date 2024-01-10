import matplotlib.pyplot as plt
from subprocess import check_output, DEVNULL
import re


port = 8080
timeout = 2.0
number_of_tests = 60
step = 100
duration_in_seconds = 1

x, y = [], []

for rate in range(step, number_of_tests * step + 1 , step ):
    print(f"Run [{int(rate/step)}/{number_of_tests}] : ", end = " ")

    cmd = [
        "httperf", "--hog",
        "--port", f"{port}",
        "--num-conns", f"{int(rate / duration_in_seconds)}",
        "--rate", f"{rate}",
        "--timeout", "2"
    ]

    #
    perf_out = check_output(cmd, stderr=DEVNULL).decode()

    match_str = re.search(r"(?<=Request rate: ).*", perf_out)[0]
    req_p_s, ms_p_req = re.findall(r"\d*?\.\d*", match_str)

    match_str = re.search(r"(?<=Total: ).*test", perf_out)[0]
    connections, requests, replies = re.findall(r"\d{1,10}", match_str)
    print(f"conns :{connections}, reqs :{requests}, repl: {replies}", end=" ")

    print(f"{req_p_s} req/s | {ms_p_req} ms/req\n")

    x.append(rate)
    y.append(float(req_p_s))


plt.plot(x, y, linestyle = "", marker = "+")

plt.title("Cweb Performance")
plt.xlabel("target req/s")
plt.ylabel("actual req/s")

plt.savefig("perf/cweb_perf.svgz");
plt.show()
