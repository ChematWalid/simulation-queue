import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("simulation_results.csv")

rho = df["rho"]
arr_time = df["arrival_time"]
serv_time = df["service_time"].iloc[0]
mu = 1.0 / serv_time
lam = 1.0 / arr_time

W_sim = df["mean_response_time"]
Wq_sim = df["mean_waiting_time"]
throughput_sim = df["throughput"]

L_sim = throughput_sim * W_sim
Lq_sim = throughput_sim * Wq_sim

W_theory = 1.0 / (mu * (1.0 - rho))
Wq_theory = rho / (mu * (1.0 - rho))
L_theory = rho / (1.0 - rho)
Lq_theory = (rho ** 2) / (1.0 - rho)
throughput_theory = lam

fig, axes = plt.subplots(1, 3, figsize=(18, 5.5))

axes[0].plot(rho, W_sim, "o-", label="Simulated W (Response)", color="tab:blue")
axes[0].plot(rho, Wq_sim, "s-", label="Simulated Wq (Waiting)", color="tab:orange")
axes[0].plot(rho, W_theory, "--", label="Theoretical W", color="darkblue", alpha=0.7)
axes[0].plot(rho, Wq_theory, "--", label="Theoretical Wq", color="darkred", alpha=0.7)
axes[0].set_title("Delays: Response & Waiting Time", fontsize=12)
axes[0].set_xlabel("Server Load (ρ = λ / μ)", fontsize=11)
axes[0].set_ylabel("Time (seconds)", fontsize=11)
axes[0].grid(True, linestyle="--", alpha=0.6)
axes[0].legend(fontsize=9)

axes[1].plot(rho, L_sim, "o-", label="Simulated L (In System)", color="tab:purple")
axes[1].plot(rho, Lq_sim, "s-", label="Simulated Lq (In Queue)", color="tab:red")
axes[1].plot(rho, L_theory, "--", label="Theoretical L = ρ/(1-ρ)", color="indigo", alpha=0.7)
axes[1].plot(rho, Lq_theory, "--", label="Theoretical Lq = ρ²/(1-ρ)", color="darkred", alpha=0.7)
axes[1].set_title("Queue Length: Clients in System & Queue", fontsize=12)
axes[1].set_xlabel("Server Load (ρ = λ / μ)", fontsize=11)
axes[1].set_ylabel("Number of Clients", fontsize=11)
axes[1].grid(True, linestyle="--", alpha=0.6)
axes[1].legend(fontsize=9)

axes[2].plot(rho, throughput_sim, "o-", label="Simulated Throughput", color="tab:green")
axes[2].plot(rho, throughput_theory, "--", label="Theoretical Throughput (λ)", color="darkgreen", alpha=0.7)
axes[2].set_title("Throughput vs Server Load", fontsize=12)
axes[2].set_xlabel("Server Load (ρ = λ / μ)", fontsize=11)
axes[2].set_ylabel("Clients per second", fontsize=11)
axes[2].grid(True, linestyle="--", alpha=0.6)
axes[2].legend(fontsize=9)

plt.suptitle("M/M/1 Queue: Complete Performance Evaluation", fontsize=14, y=1.02)
plt.tight_layout()

dashboard_file = "mm1_queue_delays_length_and_throughput.png"
plt.savefig(dashboard_file, dpi=150, bbox_inches="tight")
print(f"Combined dashboard saved to: {dashboard_file}")

plt.figure(figsize=(8, 5))
plt.plot(rho, L_sim, "o-", label="Simulated L (System)", color="tab:purple")
plt.plot(rho, Lq_sim, "s-", label="Simulated Lq (Queue)", color="tab:red")
plt.plot(rho, L_theory, "--", label="Theoretical L = ρ/(1-ρ)", color="indigo", alpha=0.7)
plt.plot(rho, Lq_theory, "--", label="Theoretical Lq = ρ²/(1-ρ)", color="darkred", alpha=0.7)
plt.xlabel("Server Load (ρ = λ / μ)", fontsize=11)
plt.ylabel("Average Number of Clients", fontsize=11)
plt.title("M/M/1 Queue Length vs Server Load", fontsize=13)
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend(fontsize=10)
plt.tight_layout()

length_file = "mm1_queue_length_vs_load.png"
plt.savefig(length_file, dpi=150)
print(f"Queue length plot saved to: {length_file}")

plt.figure(figsize=(8, 5))
plt.plot(rho, throughput_sim, "o-", label="Simulated Throughput", color="tab:green")
plt.plot(rho, throughput_theory, "--", label="Theoretical Throughput (λ)", color="darkgreen", alpha=0.7)
plt.xlabel("Server Load (ρ = λ / μ)", fontsize=11)
plt.ylabel("Throughput (Clients / second)", fontsize=11)
plt.title("M/M/1 Throughput vs Server Load", fontsize=13)
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend(fontsize=10)
plt.tight_layout()

throughput_file = "mm1_throughput_vs_load.png"
plt.savefig(throughput_file, dpi=150)
print(f"Throughput plot saved to: {throughput_file}")
