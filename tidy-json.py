#!/usr/bin/env python
import yaml
import json
import os

def getLineFromOffset(fileName, offset):
	with open(fileName) as f:
		text = f.read(offset)
		return text.count('\n') + 1
	return 0
	
def main():
	cwd = os.getcwd()
	print(cwd)
	with open('build/tidy.yaml') as f:
		data = yaml.safe_load(f)
		if not data:
			return

		#print(data)
		jsonOut = []
		for diagnostic in data['Diagnostics']:
			filePath = diagnostic['DiagnosticMessage']['FilePath']
			filePath = filePath.replace(cwd + '/', '')
			
			jsonOut.append({
				'file': filePath,
				'line': getLineFromOffset(filePath, diagnostic['DiagnosticMessage']['FileOffset']),
				'title': diagnostic['DiagnosticName'],
				'message': diagnostic['DiagnosticMessage']['Message'],
				'annotation_level': 'warning'
			})
		jsonStr = json.dumps(jsonOut, indent=2)
		#print(jsonStr)
		with open('tidy-annotations.json', 'w') as outFile:
			outFile.write(jsonStr)
	
if __name__ == '__main__':
	main()