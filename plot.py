import matplotlib.pyplot as plt

times = []
kinetic_sum = []
potential_sum = []
energy_sum = []

with open("energy_output.txt") as file:
    for line in file:
        times.append(float(line.split()[0]))
        kinetic_sum.append(float(line.split()[2]))
        potential_sum.append(float(line.split()[1]))
        energy_sum.append(float(line.split()[3]))

# Font settings
plt.rcParams.update(
    {
        "font.family": "Avenir",
        "mathtext.fontset": "stix",
    }
)

# Definig plot

fig = plt.figure(figsize=(8, 4))
ax1 = fig.add_subplot()

ax1.plot(times, energy_sum, "-", color="black", label=r"$\Sigma E$")
ax1.plot(times, kinetic_sum, "-", color="red", label=r"$\Sigma K$", alpha=0.5)
ax1.plot(times, potential_sum, "-", color="blue", label=r"$\Sigma U$", alpha=0.5)


# Labelling
ax1.set_title("Energy over Time")
ax1.set_xlabel("$t$ (s)")
ax1.set_ylabel("Energy")
ax1.legend(loc="lower right", framealpha=0.5)
ax1.grid(True)


plt.savefig("energy_plot.png", dpi=300)
plt.show()
