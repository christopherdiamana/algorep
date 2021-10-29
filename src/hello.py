from mpi4py import MPI


rank = 0
size = 0
length = 0
version = ""

#MPI.Init()
rank = MPI.COMM_WORLD.Get_rank()
size = MPI.COMM_WORLD.Get_size()
version = MPI.Get_library_version()
name = MPI.Get_processor_name()

print("Hello world, I am {} of {} on {}, ({})".format(rank, size, name, version))

MPI.Finalize()

