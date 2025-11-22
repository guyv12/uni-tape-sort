import math
import numpy as np
import matplotlib.pyplot as plt


def sort_params(file_name: str) -> tuple[int, int]:
    with open(file_name) as f:
        n, b = map(int, f.readline().split())

    return n, b
    

def load_arrays(file_name: str) -> tuple[np.ndarray, np.ndarray]:
    try:
        fsizes, operations = np.loadtxt(
            file_name,
            delimiter=' ',
            skiprows=1,
            usecols=(0, 1),
            unpack=True,
            dtype=int
        )
        return fsizes, operations
    
    except:
        print("Error opening file")
        return np.array([], dtype=int), np.array([], dtype=int)
    

def approx_ops(n: int, b: int, fsizes: np.ndarray) -> np.ndarray:
    return np.asarray([(2 * N / (b * math.log2(n))) * math.log2(N / b) for N in fsizes])


def plot_arrays(fsizes: np.ndarray, operations: np.ndarray, approx: np.ndarray) -> None:
    plt.figure(figsize=(10, 6))

    plt.plot(operations, fsizes, color='r', label='actual')
    plt.plot(approx, fsizes, color='b', label='approximated')
    plt.xlabel("R/W operations", fontsize=12)
    plt.ylabel("File size (records)", fontsize=12)
    plt.title("Operations vs. File Size", fontsize=14)
    
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.legend()
    
    plt.show()
    plt.savefig('sort.png')
    

def main() -> None:

    file_name = 'sort.dat'

    b, n = sort_params(file_name)
    fsizes, operations = load_arrays(file_name)
    approx = approx_ops(n, b, fsizes)

    plot_arrays(fsizes, operations, approx)


if __name__ == '__main__':
    main()