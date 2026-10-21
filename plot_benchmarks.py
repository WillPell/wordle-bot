# AI GENERATED GRAPHING SCRIPT

import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator, PercentFormatter


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "docs" / "images"
COLORS = {"Frequency": "#BA8D22", "Entropy": "#408653"}


def load_results(strategy):
    with (ROOT / "results" / f"{strategy}.csv").open(newline="") as source:
        rows = list(csv.DictReader(source))
    counts = [0] * 7  # Zero represents failures, not zero-guess successes.
    for row in rows:
        if row["solved"] == "true":
            guesses = int(row["guesses"])
            if not 1 <= guesses <= 6:
                raise ValueError(f"Invalid guess count: {row}")
            counts[guesses] += 1
        elif row["solved"] == "false" and row["guesses"] == "":
            counts[0] += 1
        else:
            raise ValueError(f"Invalid result: {row}")
    return rows, counts


def style_axis(ax):
    ax.spines[["top", "right"]].set_visible(False)
    ax.spines[["left", "bottom"]].set_color("#D6DBD5")
    ax.grid(axis="y", color="#E8ECE7", linewidth=0.8)
    ax.set_axisbelow(True)
    ax.tick_params(length=0, pad=8)


def save(fig, name):
    fig.savefig(OUTPUT / name, dpi=200, facecolor="white")
    plt.close(fig)


def main():
    frequency_rows, frequency = load_results("frequency")
    entropy_rows, entropy = load_results("entropy")
    answers = [row["answer"] for row in frequency_rows]
    if not answers or len(set(answers)) != len(answers):
        raise ValueError("Expected a nonempty set of unique answers")
    if answers != [row["answer"] for row in entropy_rows]:
        raise ValueError("Both strategies must use the same ordered answer set")
    total = len(answers)
    OUTPUT.mkdir(parents=True, exist_ok=True)
    plt.rcParams.update({
        "font.family": "DejaVu Sans", "font.size": 11,
        "text.color": "#25312A", "axes.labelcolor": "#47544B",
        "xtick.color": "#47544B", "ytick.color": "#47544B",
        "axes.titlesize": 19, "axes.titleweight": "bold",
        "legend.frameon": False,
    })

    fig, ax = plt.subplots(figsize=(10, 5.5), layout="constrained")
    positions = list(range(7))
    for offset, (name, counts) in zip((-0.2, 0.2),
                                     (("Frequency", frequency), ("Entropy", entropy))):
        values = counts[1:] + counts[:1]
        bars = ax.bar([x + offset for x in positions], values, width=0.36,
                      label=name, color=COLORS[name], zorder=3)
        ax.bar_label(bars, labels=[f"{n:,}" if n else "" for n in values],
                     padding=4, fontsize=9)
    ax.set_xticks(positions, ["1", "2", "3", "4", "5", "6", "Failed"])
    ax.set_xlabel("Guesses to solve (failures exhausted six guesses)")
    ax.set_ylabel("Number of answers")
    ax.set_title("Entropy shifts more games into 3–4 guesses", loc="left", pad=38)
    ax.text(0, 1.035, f"{total:,} answers · identical word lists · six-guess limit",
            transform=ax.transAxes, color="#647168")
    ax.set_ylim(0, max(max(frequency), max(entropy)) * 1.22)
    ax.yaxis.set_major_locator(MaxNLocator(integer=True))
    ax.legend(loc="upper right", ncol=2)
    style_axis(ax)
    save(fig, "guess-distribution.png")

    fig, ax = plt.subplots(figsize=(10, 5.5), layout="constrained")
    for name, counts, label_offset in (("Frequency", frequency, -17),
                                       ("Entropy", entropy, 14)):
        cumulative = [100 * sum(counts[1:turn + 1]) / total for turn in range(7)]
        ax.plot(range(7), cumulative, marker="o", linewidth=2.5,
                markersize=6, label=name, color=COLORS[name])
        ax.annotate(f"{name}: {cumulative[-1]:.2f}%", (6, cumulative[-1]),
                    xytext=(-8, label_offset), textcoords="offset points",
                    ha="right", color=COLORS[name], weight="bold")
    ax.set_title("Solved within the six-guess limit", loc="left", pad=38)
    ax.text(0, 1.035, "Share of all answers solved by each turn; failures stay in the denominator",
            transform=ax.transAxes, color="#647168")
    ax.set_xlabel("Guesses used")
    ax.set_ylabel("Cumulative share solved")
    ax.set_xticks(range(7))
    ax.set_xlim(-0.1, 6.2)
    ax.set_ylim(0, 108)
    ax.set_yticks(range(0, 101, 20))
    ax.yaxis.set_major_formatter(PercentFormatter(100))
    ax.legend(loc="upper left")
    style_axis(ax)
    save(fig, "cumulative-success.png")
    print(f"Created two figures from {total:,} answers in {OUTPUT}")


if __name__ == "__main__":
    main()
