call %%PYTHON%% make_kit_files_list.py kitfiles >bemacs-kitfiles.iss
call "c:\Program Files (x86)\Inno Setup 5\ISCC.exe" bemacs.iss
copy Output\setup.exe bemacs-%(version)s-setup.exe
dir/s/b bemacs-%(version)s-setup.exe
