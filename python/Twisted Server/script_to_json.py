# -*- coding: utf-8 -*-

import os, stat, sys
from twisted.python import log
from twisted.internet import reactor
import json, uuid, re

from sim_subprocess import SubProcessProtocol

class Parser:

	script_name = None

	def __init__(self, script_str):
		self.script_name = str(uuid.uuid4()) + '.py'
		script_file = open(self.script_name, 'w')
		script_file.write(script_str)
		script_file.close()

	def modify_script_file(self):
            input = open(self.script_name)
            line_list = input.readlines()
            input.close()

            # get sim variable name and report path
            sim = ''
            report_path = ''
            for line in line_list:
            	if 'ncs.Simulation()' in line:
            		space_free = re.sub(r'\s', '', line)
            		sim = space_free.split('=ncs.Simulation()')[0]

            	elif 'toAsciiFile' in line:
            		report_path = re.findall(r'"(.*?)"', line)[0]

            # replace build function calls with parse functions
            insert_index = None
            num_tabs = 0
            new_line_list = []
            for index, line in enumerate(line_list):
                if sim + '.addNeuron' in line:
                    new_line_list.append(line.replace('addNeuron', 'parseNeuron'))
                elif sim + '.addSynapse' in line:
                    new_line_list.append(line.replace('addSynapse', 'parseSynapse'))
                elif sim + '.init' in line:
                    new_line_list.append(line.replace('init', 'parseInit'))
                elif sim + '.addStimulus' in line:
                    new_line_list.append(line.replace('addStimulus', 'parseStimulus'))
                elif sim + '.addReport' in line:
                    new_line_list.append(line.replace('addReport', 'parseReport'))
                elif 'toAsciiFile' in line:
                    new_line_list.append(line.replace('toAsciiFile', 'parseToAsciiFile'))
            	elif sim + '.run' in line:
                    new_line_list.append(line.replace('run', 'parseRun'))
		    insert_index = index + 1
		    num_tabs = len(line) - len(line.lstrip('\t'))
                else:
                    new_line_list.append(line)

            # append appropriate number of tabs
            tabs = ''
            for i in range(num_tabs):
            	tabs += '\t'

            # insert lines that will output the sim structure to a file
            new_line_list[insert_index:insert_index] = [tabs + 'sim_spec = {}\n']
            insert_index += 1
            new_line_list[insert_index:insert_index] = [tabs + 'sim_spec["inputs"] = sim.parse_stim_spec\n']
            insert_index += 1
            new_line_list[insert_index:insert_index] = [tabs + 'sim_spec["outputs"] = sim.parse_report_spec\n']
            insert_index += 1
            new_line_list[insert_index:insert_index] = [tabs + 'sim_spec["run"] = sim.parse_run_spec\n']
            insert_index += 1
    	    new_line_list[insert_index:insert_index] = [tabs + 'sim_json = {"model": sim.parse_model_spec, "simulation": sim_spec}\n']
            insert_index += 1
    	    new_line_list[insert_index:insert_index] = [tabs + 'json_file = open("' + self.script_name.split('.py')[0] + '.json", "w")\n']
            insert_index += 1
    	    new_line_list[insert_index:insert_index] = [tabs + 'json_file.write(json.dumps(sim_json, sort_keys=True, indent=2) + "\\n\\n\\n")']
            new_line_list[:0] = ['import json\n']

	    # create modified file
	    script_file = open(self.script_name, "w")
	    for line in new_line_list:
	        script_file.write(line)
	    script_file.close()

	def create_json_file(self):

		st = os.stat(self.script_name)
		os.chmod(self.script_name, st.st_mode | stat.S_IEXEC)

		# set the subprocess environment to the location of pyncs
		pp = SubProcessProtocol()
		pyncs_env = os.environ.copy()
		pyncs_env["PATH"] = '../../../../' + pyncs_env["PATH"]

		# run the simulation script as a subprocess
		reactor.spawnProcess(pp, "./" + self.script_name, env=pyncs_env)
		
	def build_ncb_json(self):

		'''
		with open('/path/to/my_file.json', 'r') as f:
		    try:
		        data = json.load(f)
		    # if the file is empty the ValueError will be thrown
		    except ValueError:
		        data = {}
		'''

        sim_params = {
                      "model": {
                        "author": "", 
                        "cellAliases": [], 
                        "cellGroups": {
                          "cellGroups": [], 
                          "classification": "cellGroup", 
                          "description": "Description", 
                          "name": "Home"
                        }, 
                        "classification": "model", 
                        "description": "Description", 
                        "name": "Current Model", 
                        "synapses": []
                      }, 
                      "simulation": {
                        "duration": None, 
                        "fsv": None, 
                        "includeDistance": "No", 
                        "inputs": [], 
                        "interactive": "No", 
                        "name": "sim", 
                        "outputs": [], 
                        "seed": None
                      }
                    }
        model = sim_params['model']
        sim = sim_params['simulation']

        neurons = model['cellGroups']['cellGroups']
        synapses = model['synapses']
        stimuli = sim['inputs']
        reports = sim['outputs']

	def delete_files(self):
		try:
			os.remove(self.script_name)
			print 'Removed script file.'
		except OSError:
			log.msg("Could not delete script file.")

		json_file_name = self.script_name.split('.py')[0] + 'json'
		try:
			os.remove(json_file_name)
			print 'Removed json file.'
		except OSError:
			log.msg("Could not delete json file.")

# testing testing
if __name__ == "__main__":

	input = open('regular_spiking_izh_test.py')
	text = input.read()
	input.close()

	parser = Parser(text)

	parser.modify_script_file()
