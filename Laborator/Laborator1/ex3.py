import requests
import json

session = requests.Session()

payload = {"username":"sprc", "password":"admin", "nume":"Verna"}
r = session.post('https://sprc.dfilip.xyz/lab1/task3/login',
				  data=json.dumps(payload))

print(r.text)
print(r.status_code)
print(session.cookies.get_dict()['session'])

cookie = session.cookies.get_dict()
r = session.get('https://sprc.dfilip.xyz/lab1/task3/check',
				cookies=cookie)

print(r.text)
print(r.status_code)

"""
Login succesful! You will be recognised as Verna for 10 sec.
200
1eb77636c73147b7b0a1e631e28eb3c7
{"status":"ok","proof":"eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ3aG8iOnsibnVtZSI6IlZlcm5hIn0sImRpZCI6InRhc2szIn0.GhpMvvBHX_1G_VkWJuQ9xP8ERsX5QiPVP4i2AWC1bJY"}
200
"""