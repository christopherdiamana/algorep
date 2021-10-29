NBPROC = 2

venv/bin/activate: requirements.txt
	python3 -m venv venv
	./venv/bin/pip install -r requirements.txt

run: venv/bin/activate
	#mpirun -n $(NBPROC) -hostfile src/hostfile python src/hello.py
	mpirun -n $(NBPROC) -hostfile src/hostfile ./venv/bin/python3  src/hello.py

clean:
	rm -rf __pycache__
	rm -rf venv
