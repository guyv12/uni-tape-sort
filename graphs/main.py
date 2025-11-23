import math
import numpy as np
import matplotlib.pyplot as plt


def sort_params(file_name: str) -> tuple[int, int]:
    with open(file_name) as f:
        n, b = map(int, f.readline().split())

    return n, b


def load_arrays(file_name: str) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    try:

        fsizes, operations, sort_phases, merge_phases, times = np.loadtxt(
            file_name,
            delimiter=' ',
            skiprows=1,
            usecols=(0, 1, 2, 3, 4),
            unpack=True,
            dtype=[('fsizes', 'i4'), ('operations', 'i4'), ('sort_phases', 'i4'), ('merge_phases', 'i4'), ('times', 'f8')]
        )
        return fsizes, operations, sort_phases, merge_phases, times

    except Exception as e:
        print(f"Error opening file: {e}")
        return np.array([], dtype=int), np.array([], dtype=int), np.array([], dtype=int), np.array([], dtype=int), np.array([], dtype=float)


def approx_ops(n: int, b: int, fsizes: np.ndarray) -> np.ndarray:
    return np.asarray([(2 * N / (b * math.log2(n))) * math.log2(N / b) for N in fsizes])

def approx_ops2(n: int, b: int, fsizes: np.ndarray) -> np.ndarray:
    return np.asarray([2 * math.ceil(N / b) * (1 + math.ceil(math.log((N / (n * b)), n - 1))) for N in fsizes])


def plot_rw(fsizes: np.ndarray, operations: np.ndarray, approx: np.ndarray) -> None:
    plt.figure(figsize=(10, 6))

    plt.plot(fsizes, operations, color='r', label='actual')
    plt.plot(fsizes, approx, color='b', label='approximated')
    plt.xlabel("File size [records]", fontsize=12)
    plt.ylabel("R/W operations", fontsize=12)
    plt.title("Operations vs. File Size", fontsize=14)
    
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.legend()
    
    plt.show()
    plt.savefig('rw.png')
    

def plot_phases(fsizes: np.ndarray, s_phases: np.ndarray, m_phases: np.ndarray) -> None:
    plt.figure(figsize=(10, 6))

    plt.plot(fsizes, s_phases, color='r', label='sort phases')
    plt.xlabel("File size [records]", fontsize=12)
    plt.ylabel("Sort Phases", fontsize=12)
    plt.title("Sort phases vs. File Size", fontsize=14)

    plt.legend()
    
    plt.show()
    plt.savefig('s_phases.png')


    plt.figure(figsize=(10, 6))

    plt.step(fsizes, m_phases, color='b', label='merge phases')
    plt.xlabel("File size [records]", fontsize=12)
    plt.ylabel("Merge Phases", fontsize=12)
    plt.title("Merge phases vs. File Size", fontsize=14)

    plt.legend()
    
    plt.show()
    plt.savefig('m_phases.png')


def plot_time(fsizes: np.ndarray, times: np.ndarray) -> None:
    plt.figure(figsize=(10, 6))

    plt.plot(fsizes, times, color='orange', label='execution time')
    plt.xlabel("File size (records)", fontsize=12)
    plt.ylabel("Time [ms]", fontsize=12)
    plt.title("Time vs. File Size", fontsize=14)
    plt.legend()
    
    plt.show()
    plt.savefig('time.png')


def main() -> None:

    file_name = 'sort.dat'

    b, n = sort_params(file_name)
    fsizes, operations, s_phases, m_phases, times = load_arrays(file_name)
    approx = approx_ops(n, b, fsizes)

    plot_rw(fsizes, operations, approx)
    plot_time(fsizes, times)
    plot_phases(fsizes, s_phases, m_phases)


if __name__ == '__main__':
    main()