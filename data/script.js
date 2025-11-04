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
        alert('Error controlling device');
    }
}

async function addDevice(event) {
    event.preventDefault();

    const mac = document.getElementById('deviceMAC').value.trim().toUpperCase();
    const name = document.getElementById('deviceName').value.trim();

    // Validate MAC address format
    const macPattern = /^([0-9A-F]{2}[:]){5}([0-9A-F]{2})$/;
    if (!macPattern.test(mac)) {
        alert('Invalid MAC address format. Use format: AA:BB:CC:DD:EE:FF');
        return;
    }

    try {
        const response = await fetch('/api/device/add', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ mac, name })
        });

        const data = await response.json();

        if (response.ok) {
            alert(data.message || 'Device added successfully! IP address will be discovered automatically.');
            document.getElementById('addDeviceForm').reset();

            // Trigger discovery to find the device IP immediately
            await startDiscoveryQuiet();

            // Reload devices list
            await loadDevices();
        } else {
            alert(data.error || 'Failed to add device');
        }
    } catch (error) {
        console.error('Add device error:', error);
        alert('Error adding device');
    }
}

async function startDiscovery() {
    const btn = event.target;
    btn.disabled = true;
    btn.textContent = 'Scanning...';

    try {
        const response = await fetch('/api/discovery/scan', { method: 'POST' });
        const data = await response.json();

        alert(`Discovery complete! Found ${data.discovered} device(s)`);
        await loadDevices();
    } catch (error) {
        console.error('Discovery error:', error);
        alert('Discovery failed');
    } finally {
        btn.disabled = false;
        btn.textContent = 'Scan for Devices';
    }
}

async function startDiscoveryQuiet() {
    try {
        await fetch('/api/discovery/scan', { method: 'POST' });
    } catch (error) {
        console.error('Background discovery error:', error);
    }
}

// Rules
async function loadRules() {
    try {
        const response = await fetch('/api/rules');
        const data = await response.json();
        rules = data.rules || [];
        displayRules();
    } catch (error) {
        console.error('Failed to load rules:', error);
        document.getElementById('rulesList').innerHTML = '<p class="loading">Error loading rules</p>';
    }
}

function displayRules() {
    const container = document.getElementById('rulesList');

    if (rules.length === 0) {
        container.innerHTML = '<p class="loading">No rules configured. Click "Add Rule" to create one.</p>';
        return;
    }

    let html = '';
    rules.forEach(rule => {
        html += `
            <div class="rule-item">
                <div class="rule-info">
                    <div class="rule-name">${rule.id}</div>
                    <div class="rule-details">
                        Device: ${rule.deviceId} | Condition: ${rule.condition}<br>
                        Threshold: ${rule.threshold} W | Hysteresis: ${(rule.hysteresis * 100).toFixed(0)}%<br>
                        Delay ON: ${rule.delayOn / 1000}s | Delay OFF: ${rule.delayOff / 1000}s<br>
                        Status: ${rule.enabled ? '<span class="status-online">Enabled</span>' : '<span class="status-offline">Disabled</span>'}
                    </div>
                </div>
                <div class="rule-actions">
                    <button class="btn btn-secondary btn-small" onclick="toggleRule('${rule.id}')">
                        ${rule.enabled ? 'Disable' : 'Enable'}
                    </button>
                </div>
            </div>
        `;
    });

    container.innerHTML = html;
}

function showAddRuleDialog() {
    alert('Rule editor coming soon! For now, configure rules via backend API or configuration file.');
}

function toggleRule(ruleId) {
    alert('Rule toggle coming soon!');
}

// WiFi Configuration
async function saveWiFi(event) {
    event.preventDefault();

    const ssid = document.getElementById('ssid').value;
    const password = document.getElementById('password').value;

    try {
        const response = await fetch('/api/wifi/config', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ ssid, password })
        });

        if (response.ok) {
            alert('WiFi configuration saved! Device will attempt to connect...');
            setTimeout(() => {
                window.location.reload();
            }, 5000);
        } else {
            alert('Failed to save WiFi configuration');
        }
    } catch (error) {
        console.error('WiFi config error:', error);
        alert('Error saving WiFi configuration');
    }
}

async function saveBackend(event) {
    event.preventDefault();
    alert('Backend configuration coming soon!');
}

// Utility functions
function refreshAll() {
    loadSystemStatus();
    loadDevices();
    loadRules();
}
