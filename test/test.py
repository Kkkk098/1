import numpy as np
import subprocess

def test_c_openmp(): 
    return 0

def test_c_pthreads():
    return 0

# Функция для выполнения скомпилированного C файла
def run_executable(executable_file_args: list):
    try:
        result = subprocess.run(executable_file_args, check=True, text=True, capture_output=True)
        print(f"Output of the executable:\n{result.stdout}")
    except subprocess.CalledProcessError as e:
        print(f"Error while running the executable: {e}")
        print(f"Error output:\n{e.stderr}")
        
# Функция для генерации простых чисел с использованием NumPy
def generate_primes(limit):
    sieve = np.ones(limit, dtype=bool)
    sieve[:2] = False
    for start in range(2, int(limit ** 0.5) + 1):
        if sieve[start]:
            sieve[start*start:limit:start] = False
    primes = np.nonzero(sieve)[0]
    return primes


def test_mpi_case(numbers_count):
    run_executable(["mpiexec", "-n", "4", "./res_mpi.out", str(numbers_count)])

    primes = generate_primes(numbers_count).tolist()

    file_path = "./results/res_mpi.txt"
    with open(file_path, 'r') as file:
        file_numbers = [int(line.strip()) for line in file.readlines()]

    assert file_numbers == primes, f"Result for {numbers_count} numbers is not correct"

    print(f"Test case C MPI Completed for {numbers_count} numbers ✅")

def test_c_mpi():
    test_mpi_case(100)
    test_mpi_case(1000)

    return 0

def main():
    test_c_mpi()

if __name__ == '__main__':
    main()

