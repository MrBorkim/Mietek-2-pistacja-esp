// Pistachio Smart Controller - JavaScript

// Global state
let devices = [];
let rules = [];
let systemStatus = {};
let isAPMode = false;

// Initialize on page load
document.addEventListener('DOMContentLoaded', function() {
    initTabs();
    loadSystemStatus();
    loadDevices();
    loadRules();

    // Check if in AP mode and show banner
    checkAPMode();

    // Auto-refresh every 30 seconds
    setInterval(loadSystemStatus, 30000);
    setInterval(loadDevices, 30000);
});

// Tab switching
function initTabs() {
    const tabButtons = document.querySelectorAll('.tab-btn');
    tabButtons.forEach(btn => {
        btn.addEventListener('click', function() {
            const tabName = this.dataset.tab;
            switchTab(tabName);
        });
    });
}

function switchTab(tabName) {
    // Update buttons
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.classList.remove('active');
        if (btn.dataset.tab === tabName) {
            btn.classList.add('active');
        }
    });

    // Update content
    document.querySelectorAll('.tab-content').forEach(content => {
        content.classList.remove('active');
    });
    document.getElementById(tabName).classList.add('active');
}

// System Status
async function loadSystemStatus() {
    try {
        const response = await fetch('/api/status');
        const data = await response.json();
        systemStatus = data;
        isAPMode = data.wifi.ap_mode || false;
        updateStatusDisplay(data);
        updateAPModeBanner();
    } catch (error) {
        console.error('Failed to load system status:', error);
        document.getElementById('wifiStatus').textContent = 'Error';
    }
}

// Check and display AP mode banner
function checkAPMode() {
    if (isAPMode) {
        showAPModeBanner();
    }
}

function updateAPModeBanner() {
    if (isAPMode) {
        showAPModeBanner();
    } else {
        hideAPModeBanner();
    }
}

function showAPModeBanner() {
    let banner = document.getElementById('apModeBanner');
    if (!banner) {
        banner = document.createElement('div');
        banner.id = 'apModeBanner';
        banner.style.cssText = `
            position: fixed;
            top: 0;
            left: 0;
            right: 0;
            background: linear-gradient(135deg, #f59e0b, #d97706);
            color: white;
            padding: 1rem;
            text-align: center;
            z-index: 9999;
            font-weight: 600;
            box-shadow: 0 4px 12px rgba(0,0,0,0.3);
        `;
        banner.innerHTML = `
            ⚠️ <strong>ACCESS POINT MODE</strong> - Configure WiFi in Settings to connect to your network
            <button onclick="switchTab('settings'); hideAPModeBanner();" style="
                margin-left: 1rem;
                padding: 0.5rem 1rem;
                background: white;
                color: #d97706;
                border: none;
                border-radius: 0.5rem;
                font-weight: 600;
                cursor: pointer;
            ">Configure WiFi →</button>
        `;
        document.body.insertBefore(banner, document.body.firstChild);

        // Adjust body padding to account for banner
        document.body.style.paddingTop = '4rem';
    }
}

function hideAPModeBanner() {
    const banner = document.getElementById('apModeBanner');
    if (banner) {
        banner.remove();
        document.body.style.paddingTop = '0';
    }
}

function updateStatusDisplay(status) {
    // Status bar
    let wifiStatus;
    if (status.wifi.ap_mode) {
        wifiStatus = '📶 AP Mode - Not connected';
    } else if (status.wifi.connected) {
        wifiStatus = `✅ ${status.wifi.ssid} (${status.wifi.rssi} dBm)`;
    } else {
        wifiStatus = '❌ Disconnected';
    }
    document.getElementById('wifiStatus').textContent = wifiStatus;
    document.getElementById('deviceCount').textContent = `${devices.length} devices`;

    // Dashboard
    document.getElementById('dashWifi').textContent = status.wifi.ssid || 'Not connected';
    document.getElementById('dashIP').textContent = status.wifi.ip || 'N/A';
    document.getElementById('dashUptime').textContent = formatUptime(status.uptime);

    const memoryPercent = ((status.storage.total - status.storage.used) / status.storage.total * 100).toFixed(1);
    document.getElementById('dashMemory').textContent = `${memoryPercent}% free`;
}

function formatUptime(seconds) {
    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const mins = Math.floor((seconds % 3600) / 60);

    if (days > 0) {
        return `${days}d ${hours}h ${mins}m`;
    } else if (hours > 0) {
        return `${hours}h ${mins}m`;
    } else {
        return `${mins}m`;
    }
}

// Devices
async function loadDevices() {
    try {
        const response = await fetch('/api/devices');
        const data = await response.json();
        devices = data.devices || [];
        displayDevices();
    } catch (error) {
        console.error('Failed to load devices:', error);
        document.getElementById('devicesList').innerHTML = '<p class="loading">Error loading devices</p>';
    }
}

function displayDevices() {
    const container = document.getElementById('devicesList');

    if (devices.length === 0) {
        container.innerHTML = '<p class="loading">No devices found. Click "Discover Devices" to scan.</p>';
        return;
    }

    let html = '';
    devices.forEach(device => {
        const lastSeenStr = device.lastSeen > 0 ? new Date(device.lastSeen).toLocaleString() : 'Never';
        html += `
            <div class="device-item">
                <div class="device-info">
                    <div class="device-name">${device.name}</div>
                    <div class="device-details">
                        IP: ${device.ip} | MAC: ${device.mac}<br>
                        Power: ${device.lastPower.toFixed(1)} W | Last seen: ${lastSeenStr}
                    </div>
                </div>
                <div class="device-actions">
                    <button class="btn btn-primary btn-small" onclick="controlDevice('${device.id}', true)">ON</button>
                    <button class="btn btn-secondary btn-small" onclick="controlDevice('${device.id}', false)">OFF</button>
                </div>
            </div>
        `;
    });

    container.innerHTML = html;
}

async function controlDevice(deviceId, state) {
    try {
        const response = await fetch('/api/device/control', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ deviceId, state })
        });

        if (response.ok) {
            alert(`Device ${state ? 'turned ON' : 'turned OFF'} successfully`);
            setTimeout(loadDevices, 1000);
        } else {
            alert('Failed to control device');
        }
    } catch (error) {
        console.error('Control error:', error);

// Extended functions for v2.0
function toggleTheme() {
    const theme = document.body.getAttribute('data-theme') === 'dark' ? 'light' : 'dark';
    document.body.setAttribute('data-theme', theme);
    localStorage.setItem('theme', theme);
}

function showToast(message, type = 'info') {
    console.log('[Toast]', type, message);
    // TODO: Implement toast notification
}

function startDiscovery() {
    showToast('Starting device discovery...', 'info');
    fetch('/api/discovery/scan', { method: 'POST' })
        .then(r => r.json())
        .then(data => {
            showToast('Found ' + (data.discovered || 0) + ' devices', 'success');
            setTimeout(loadDevices, 2000);
        })
        .catch(err => showToast('Discovery failed', 'error'));
}

function showManualAddDialog() {
    document.getElementById('addDeviceModal').classList.add('active');
}

function closeModal(id) {
    document.getElementById(id).classList.remove('active');
}

function showRuleBuilder() {
    document.getElementById('ruleBuilderModal').classList.add('active');
}

// Initialize on load
document.addEventListener('DOMContentLoaded', function() {
    const theme = localStorage.getItem('theme') || 'dark';
    document.body.setAttribute('data-theme', theme);
    
    // Apply theme from storage
    toggleTheme();
    toggleTheme(); // Toggle twice to apply correct theme
});

