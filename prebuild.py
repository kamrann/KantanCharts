import sys
import os
import argparse
import git


def prebuild():
	envvarname = 'KANTANROOT'
	kantanroot = os.environ.get(envvarname)
	if kantanroot is None:
		return -1

	from subprocess import call
	from git import Repo
	repo = Repo('./')
	branch = repo.active_branch.name
			
	# Copy KantanInstallation code directly into existing module
	script = os.path.join(kantanroot, '../Scripts', 'checkout_and_copy_module_subpaths.py')
	local_dest = './Source/KantanChartsEditor/Imported'
	kinstall_url = 'https://kamrann@bitbucket.org/kamrann/kantaninstallation.git'
	kinstall_modulename = 'KantanInstallation'
	pfx = 'KC'
	kinstall_subpaths = ['KantanInstallation']
	cmd = ['python', script, kinstall_url, '-b=' + branch, kinstall_modulename, '-p=' + pfx, '-d=' + local_dest, '--makereadonly'] + kinstall_subpaths
	if call(cmd) != 0:
		return -1
		
	return 0

parser = argparse.ArgumentParser()
parser.add_argument('engver', help='ue4 engine minor version')
parser.set_defaults(func=prebuild)

args = parser.parse_args()
code = args.func()
sys.exit(code)
