import numpy as np
import matplotlib.pyplot as plt


def load_arrays(file_name: str) -> tuple[np.ndarray, np.ndarray]:
    try:
        fsizes, operations = np.loadtxt(
            file_name,
            delimiter=' ',
            usecols=(0, 1),
            unpack=True,
            dtype=int
        )
        return fsizes, operations
    
    except:
        print("Error opening file")
        return np.array([], dtype=int), np.array([], dtype=int)
    

def plot_arrays(fsizes: np.ndarray, operations: np.ndarray) -> None:
    plt.figure(figsize=(10, 6))

    plt.plot(operations, fsizes)
    plt.xlabel("R/W operations", fontsize=12)
    plt.ylabel("File size (records)", fontsize=12)
    plt.title("Operations vs. File Size", fontsize=14)
    
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.legend()
    
    plt.show()
    plt.savefig('sort.png')
    

def main() -> None:

    file_name = 'sort.dat'

    fsizes, operations = load_arrays(file_name)
    plot_arrays(fsizes, operations)


if __name__ == '__main__':
    main()