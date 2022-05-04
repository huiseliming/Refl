import os
import subprocess

work_path = os.path.split(os.path.realpath(__file__))[0]
program_files_path =os.environ.get('ProgramFiles(x86)')
vswhere_path = program_files_path +'/Microsoft Visual Studio/Installer/vswhere.exe'
result = subprocess.run([vswhere_path, '-prerelease', '-latest', '-property', 'installationPath'], stdout=subprocess.PIPE)
if result.returncode != 0 :
    exit
temp_path = result.stdout.decode(os.sys.stdout.encoding).replace('\r', '').replace('\n', '')
vcvars64_path = '' + temp_path + '/VC/Auxiliary/Build/vcvars64.bat'

build_path = work_path + '/../../build'
if not os.path.exists(build_path) :
    os.makedirs(build_path)
cmake_command = 'cmd.exe /c cd /d"' + os.path.normpath(build_path) + '" & "' + os.path.normpath(vcvars64_path) + '" & cmake -G "Visual Studio 16 2019" ../llvm -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DLLVM_BUILD_TESTS=OFF'
print(cmake_command)
subprocess.call(cmake_command)
