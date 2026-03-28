import numpy as np
import matplotlib.pyplot as plt

# ==========================================
# 1. 数据加载与分离 (保持不变)
# ==========================================
file_path = 'out/build/linux-debug/fdtd_probes_results.csv' 
data = np.loadtxt(file_path, delimiter=',', skiprows=1)
time_steps = data[:, 0]
signal_in = data[:, 1]  
signal_out = data[:, 2] 
total_steps = len(time_steps)

split_index = 380 
incident_wave = np.zeros(total_steps)
incident_wave[:split_index] = signal_in[:split_index]
reflected_wave = np.zeros(total_steps)
reflected_wave[split_index:] = signal_in[split_index:]
transmitted_wave = signal_out

# ==========================================
# 2. FFT 与 能量守恒计算
# ==========================================
freq_inc = np.abs(np.fft.fft(incident_wave))
freq_ref = np.abs(np.fft.fft(reflected_wave))
freq_trans = np.abs(np.fft.fft(transmitted_wave))

half_N = total_steps // 2

# ⚠️ 注意：我们要避开第 0 个直流分量(m=0)，否则后面计算波长时除以 0 会报错
m_bins = np.arange(1, half_N) 
freq_inc = freq_inc[1:half_N]
freq_ref = freq_ref[1:half_N]
freq_trans = freq_trans[1:half_N]

R = (freq_ref / (freq_inc + 1e-10)) ** 2
T = (freq_trans / (freq_inc + 1e-10)) ** 2

# ==========================================
# 3. 【核心新增】物理量纲换算
# ==========================================
dx = 10.0 # 假设空间网格大小为 10 nm
# 绝美公式：lambda = (N * dx) / m
wavelengths_nm = (total_steps * dx) / m_bins

# ==========================================
# 4. 绘制终极物理光谱图
# ==========================================
plt.style.use('default')
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
fig.patch.set_facecolor('white')

# 时域图
ax1.plot(time_steps, incident_wave, label='Incident Wave (In)', color='blue', linestyle='--')
ax1.plot(time_steps, reflected_wave, label='Reflected Wave (Back)', color='red')
ax1.plot(time_steps, transmitted_wave, label='Transmitted Wave (Out)', color='green')
ax1.set_title(f'Time Domain Probes (N = {total_steps} steps)', color='black', fontweight='bold')
ax1.set_ylabel('Amplitude (Ez)')
ax1.legend(loc='upper right')
ax1.grid(True, linestyle=':', alpha=0.6)

# 频域图 (横坐标变成真实波长)
ax2.plot(wavelengths_nm, R, label='Reflectance (R)', color='red', linewidth=2)
ax2.plot(wavelengths_nm, T, label='Transmittance (T)', color='green', linewidth=2)
ax2.plot(wavelengths_nm, R + T, label='Total Energy (R+T)', color='black', linestyle=':', linewidth=2, alpha=0.8)

# 我们只截取 300nm 到 1000nm 的波段 (可见光到近红外)
ax2.set_xlim(300, 1000)
ax2.set_ylim(-0.05, 1.1)

ax2.set_title('Physical Spectrum: Fabry-Perot Defect Mode', color='black', fontweight='bold')
ax2.set_xlabel('Wavelength (nm)', color='black', fontsize=12, fontweight='bold')
ax2.set_ylabel('Power Ratio (0 to 1)')
ax2.legend(loc='center right')
ax2.grid(True, linestyle='--', alpha=0.7)

plt.tight_layout()
plt.show()
