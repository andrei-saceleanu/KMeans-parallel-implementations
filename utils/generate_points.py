"""Generate synthetic data for kmeans"""
import argparse
from sklearn.datasets import make_blobs


def parse_args():
    """parse CLI arguments"""

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--file",
        type=str,
        help="filename where datapoints will be written",
        required=True
    )

    parser.add_argument(
        "--point_count",
        type=int,
        help="number of datapoints to generate",
        default=500000
    )

    parser.add_argument(
        "--size",
        type=int,
        help="vector size of one datapoint",
        default=4
    )

    parser.add_argument(
        "--cluster_count",
        type=int,
        help="number of clusters/centroids",
        default=5
    )
    parser.add_argument(
        "--seed",
        type=int,
        help="seed for random generation of points",
        default=None
    )

    return parser.parse_args()

def main():
    """main function"""
    args = parse_args()

    points, _ ,centers = make_blobs(
                            n_samples=args.point_count,
                            n_features=args.size,
                            centers=args.cluster_count,
                            random_state=args.seed,
                            return_centers=True
                        )

# dump points to file and then the centroids in the same file
# after the separator line
    with open(file=args.file, mode="w", encoding="UTF-8") as fout:

        fout.write(str(args.point_count)+ " " + str(args.size) + "\n")
        for i in range(args.point_count):
            fout.write(" ".join(list(map(lambda elem:f"{elem:.2f}", points[i]))))
            fout.write("\n")

        fout.write("=====\n")

        for i in range(args.cluster_count):
            fout.write(" ".join(list(map(lambda elem:f"{elem:.2f}", centers[i]))))
            fout.write("\n")


if __name__ == "__main__":
    main()
