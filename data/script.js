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
    const tabButtons = document.querySelectorAll('.nav-btn');
    tabButtons.forEach(btn => {
        btn.addEventListener('click', function() {
            const tabName = this.dataset.tab;
            switchTab(tabName);
        });
    });
}

function switchTab(tabName) {
    // Update buttons
    document.querySelectorAll('.nav-btn').forEach(btn => {
        btn.classList.remove('active');
        if (btn.dataset.tab === tabName) {
            btn.classList.add('active');
        }
    });

    // Update content
    document.querySelectorAll('.tab-content').forEach(content => {
        content.classList.remove('active');
    });
    const targetTab = document.getElementById(tabName);
    if (targetTab) {
        targetTab.classList.add('active');
    }
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
    // Update connection status indicator
    const statusIndicator = document.querySelector('.status-indicator');
    const statusText = document.querySelector('.status-text');

    if (statusIndicator && statusText) {
        if (status.wifi.ap_mode) {
            statusIndicator.classList.remove('connected');
            statusText.textContent = 'AP Mode';
        } else if (status.wifi.connected) {
            statusIndicator.classList.add('connected');
            statusText.textContent = 'Connected';
        } else {
            statusIndicator.classList.remove('connected');
            statusText.textContent = 'Disconnected';
        }
    }

    // Update system info in settings tab
    const updateElement = (id, value) => {
        const el = document.getElementById(id);
        if (el) el.textContent = value;
    };

    updateElement('firmwareVersion', status.version || 'Unknown');
    updateElement('systemUptime', formatUptime(status.uptime || 0));
    updateElement('wifiConnected', status.wifi.ssid || 'Not connected');
    updateElement('ipAddress', status.wifi.ip || 'N/A');

    if (status.wifi.rssi) {
        updateElement('wifiRssi', `${status.wifi.rssi} dBm`);
    } else {
        updateElement('wifiRssi', 'N/A');
    }

    if (status.storage) {
        const usedKB = (status.storage.used / 1024).toFixed(1);
        const totalKB = (status.storage.total / 1024).toFixed(1);
        updateElement('freeMemory', `${usedKB} / ${totalKB} KB`);
    }

    // Update device count badge
    const deviceCountBadge = document.getElementById('deviceCountBadge');
    if (deviceCountBadge) {
        deviceCountBadge.textContent = devices.length;
    }

    // Update rule count badge
    const ruleCountBadge = document.getElementById('ruleCountBadge');
    if (ruleCountBadge) {
        ruleCountBadge.textContent = rules.length;
    }
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
    const container = document.getElementById('devicesGrid');
    if (!container) return;

    if (devices.length === 0) {
        container.innerHTML = '<div class="empty-state"><p>No devices found. Click "Auto-Discover Devices" to scan your network.</p></div>';
        return;
    }

    let html = '';
    devices.forEach(device => {
        const lastSeenStr = device.lastSeen > 0 ? new Date(device.lastSeen).toLocaleString() : 'Never';
        const statusClass = device.online ? 'online' : 'offline';
        const statusIcon = device.online ? '✓' : '○';

        html += `
            <div class="device-card glass-card">
                <div class="device-header">
                    <h3 class="device-name">${device.name}</h3>
                    <span class="device-status ${statusClass}">${statusIcon}</span>
                </div>
                <div class="device-info-grid">
                    <div class="device-info-item">
                        <span class="label">IP:</span>
                        <span class="value">${device.ip}</span>
                    </div>
                    <div class="device-info-item">
                        <span class="label">MAC:</span>
                        <span class="value">${device.mac}</span>
                    </div>
                    <div class="device-info-item">
                        <span class="label">Power:</span>
                        <span class="value">${device.lastPower ? device.lastPower.toFixed(1) + ' W' : 'N/A'}</span>
                    </div>
                    <div class="device-info-item">
                        <span class="label">Last seen:</span>
                        <span class="value">${lastSeenStr}</span>
                    </div>
                </div>
                <div class="device-actions">
                    <button class="btn btn-success btn-sm" onclick="controlDevice('${device.id}', true)">
                        <span class="btn-icon">🟢</span> ON
                    </button>
                    <button class="btn btn-secondary btn-sm" onclick="controlDevice('${device.id}', false)">
                        <span class="btn-icon">🔴</span> OFF
                    </button>
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
            showToast(`Device ${state ? 'turned ON' : 'turned OFF'} successfully`, 'success');
            setTimeout(loadDevices, 1000);
        } else {
            showToast('Failed to control device', 'error');
        }
    } catch (error) {
        console.error('Control error:', error);
        showToast('Device control error', 'error');
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
    }
}

function displayRules() {
    const container = document.getElementById('rulesList');
    if (!container) return;

    if (rules.length === 0) {
        container.innerHTML = '<p class="empty-state">No automation rules configured. Create your first rule!</p>';
        return;
    }

    let html = '';
    rules.forEach(rule => {
        html += `
            <div class="rule-item glass-card">
                <div class="rule-header">
                    <h3>${rule.name}</h3>
                    <span class="rule-status ${rule.enabled ? 'active' : 'inactive'}">
                        ${rule.enabled ? '✓ Active' : '○ Inactive'}
                    </span>
                </div>
                <p class="rule-description">${rule.description || 'No description'}</p>
            </div>
        `;
    });
    container.innerHTML = html;
}

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

