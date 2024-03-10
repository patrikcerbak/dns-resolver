import subprocess
import re


# ok test:
test1 = subprocess.run("./dns -s 1.1.1.1 google.com -r -6 -p 53", shell=True, capture_output=True, text=True)
if test1.returncode == 0:
    print("ok_test: OK")
else:
    print("ok_test: FAILED")


# missing server test:
test2 = subprocess.run("./dns google.com -r -6 -p 53", shell=True, capture_output=True, text=True)
if test2.returncode != 1:
    print("missing_server_test: FAILED")
else:
    print("missing_server_test: OK")


# missing address test:
test3 = subprocess.run("./dns -s 1.1.1.1 -r -6 -p 53", shell=True, capture_output=True, text=True)
if test3.returncode != 1:
    print("missing_address_test: FAILED")
else:
    print("missing_address_test: OK")


# wrong port test:
test4 = subprocess.run("./dns -s 1.1.1.1 -r -6 -p 999999", shell=True, capture_output=True, text=True)
if test4.returncode != 1:
    print("wrong_port_test: FAILED")
else:
    print("wrong_port_test: OK")


# wrong combination test:
test5 = subprocess.run("./dns -s 1.1.1.1 -r -6 -p 53 -x", shell=True, capture_output=True, text=True)
if test5.returncode != 1:
    print("wrong_combination_test: FAILED")
else:
    print("wrong_combination_test: OK")


# a test:
test6 = subprocess.run("./dns -s 1.1.1.1 google.com -r", shell=True, capture_output=True, text=True)
test6_correct = "Authoritative: No, Recursive: Yes, Truncated: No\n\
Question section \\(1\\)\n\
  google\\.com\\., A, IN\n\
Answer section \\(1\\)\n\
  google\\.com\\., A, IN, [0-9]*, [0-9]*\\.[0-9]*\\.[0-9]*\\.[0-9]*\n\
Authority section \\(0\\)\n\
Additional section \\(0\\)\n"
if re.match(test6_correct, test6.stdout):
    print("a_test: OK")
else:
    print("a_test: FAILED")


# aaaa test:
test7 = subprocess.run("./dns -s 1.1.1.1 google.com -r -6", shell=True, capture_output=True, text=True)
test7_correct = "Authoritative: No, Recursive: Yes, Truncated: No\n\
Question section \\(1\\)\n\
  google\\.com\\., AAAA, IN\n\
Answer section \\(1\\)\n\
  google\\.com\\., AAAA, IN, [0-9]*, [a-z0-9:]*\n\
Authority section \\(0\\)\n\
Additional section \\(0\\)\n"
if re.match(test7_correct, test7.stdout):
    print("aaaa_test: OK")
else:
    print("aaaa_test: FAILED")


# ptr test:
test8 = subprocess.run("./dns -s 1.1.1.1 142.251.37.110 -r -x", shell=True, capture_output=True, text=True)
test8_correct = "Authoritative: No, Recursive: Yes, Truncated: No\n\
Question section \\(1\\)\n\
  110\\.37\\.251\\.142\\.in-addr\\.arpa\\., PTR, IN\n\
Answer section \\(1\\)\n\
  110\\.37\\.251\\.142\\.in-addr\\.arpa\\., PTR, IN, [0-9]*, .*\n\
Authority section \\(0\\)\n\
Additional section \\(0\\)\n"
if re.match(test8_correct, test8.stdout):
    print("ptr_test: OK")
else:
    print("ptr_test: FAILED")
