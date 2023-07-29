#!/usr/bin/env python
import json
import os
	
def main():
	cwd = os.getcwd()
	print(cwd)
	with open('build/diagnostics.txt') as f:
		jsonOut = []
		while True:
			line = f.readline()
			if not line:
				break

			#print(line)
			if 'warning:' in line or 'error:' in line:
				splits = line.split(':')
				if len(splits) > 4:
					#print(splits)
					filePath = splits[0]
					filePath = filePath.replace(cwd + '/', '')
					
					iserror = False if splits[3] == " warning" else True
					
					try:
						message = ''
						message_split = []
						if iserror:
							message_split = line.split('error:')
						else:
							message_split = line.split('warning:')
						if len(message_split) > 1:
							message = message_split[1]
						
						message = message[1:-1]
						if message == '':
							message = 'Unknown warning or error, check log []'
						
						if not(iserror):
							messageSplits = message.rpartition('[')
							message = messageSplits[0]
						
						
						jsonOut.append({
							'file': filePath,
							'line': int(splits[1]),
							'title': 'Build Warning' if not (iserror) else 'Build Error',
							'message': message,
							'annotation_level': 'warning' if not(iserror) else 'failure'
						})
					except:
						jsonOut.append({
							'file': filePath,
							'line': 1,
							'title': 'Build Warning' if not (iserror) else 'Build Error',
							'message': 'Failed to generate annotation for this warning/error. Please check the actions build log.',
							'annotation_level': 'warning' if not(iserror) else 'failure'
						})
						pass	
		jsonStr = json.dumps(jsonOut, indent=2)
		#print(jsonStr)
		with open('diagnostics.json', 'w') as outFile:
			outFile.write(jsonStr)
	
if __name__ == '__main__':
	main()
