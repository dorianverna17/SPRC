import requests

headers = {'secret2': 'SPRCisBest'}
data = {'secret': 'SPRCisNice'}
r = requests.post('https://sprc.dfilip.xyz/lab1/task1/check?nume=Verna&grupa=341C1',
				  headers=headers,
				  data=data)

print(r.text)
print(r.status_code)

"""
{"status":"ok","proof":"eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ3aG8iOnsibnVtZSI6WyJWZXJuYSJdLCJncnVwYSI6WyIzNDFDMSJdfSwiZGlkIjoidGFzazEifQ.iYhUO10xQU0_Tbt0eK0RKUlG2yQlhl3DhwCw0aPbjQY"}
200
"""
