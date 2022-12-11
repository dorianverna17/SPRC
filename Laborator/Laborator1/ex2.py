import requests
import json

payload = {"username":"sprc", "password":"admin", "nume":"Verna"}
r = requests.post('https://sprc.dfilip.xyz/lab1/task2',
				  data=json.dumps(payload))

print(r.text)
print(r.status_code)

"""
{"status":"ok","proof":"eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ3aG8iOnsibnVtZSI6IlZlcm5hIn0sImRpZCI6InRhc2syIn0.aKhE9XybnT-oGAV1F_tfK-X_wo5-pnebGasKN6MPXsE"}
200
"""
