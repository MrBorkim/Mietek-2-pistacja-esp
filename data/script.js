// Pistachio Smart Controller - JavaScript

// Global state
let devices = [];
let rules = [];
let systemStatus = {};

// Initialize on page load
document.addEventListener('DOMContentLoaded', function() {
    initTabs();
    loadSystemStatus();
    loadDevices();
    loadRules();

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
        updateStatusDisplay(data);
    } catch (error) {
        console.error('Failed to load system status:', error);
        document.getElementById('wifiStatus').textContent = 'Error';
    }
}

function updateStatusDisplay(status) {
    // Status bar
    const wifiStatus = status.wifi.connected ?
        `Connected: ${status.wifi.ssid} (${status.wifi.rssi} dBm)` :
        'Disconnected';
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

