/**
 * WiFi Configuration Functions
 * Handles WiFi setup, saving, and connection status
 */

// Load current WiFi configuration
async function loadWiFiConfig() {
    try {
        const response = await fetch('/api/wifi/config');
        if (!response.ok) return;

        const data = await response.json();

        // Pre-fill form if configured
        if (data.ssid) {
            const ssidInput = document.getElementById('wifiSsid');
            if (ssidInput) ssidInput.value = data.ssid;
        }

        // Show status
        if (data.ap_mode) {
            showWiFiConfigMessage('info', '⚠️ Currently in AP Mode - Configure WiFi below to connect');
        } else if (data.configured) {
            showWiFiConfigMessage('success', `✅ Connected to: ${data.ssid}`);
        }
    } catch (error) {
        console.error('Failed to load WiFi config:', error);
    }
}

// Save WiFi configuration
async function saveWiFiConfig(event) {
    event.preventDefault();

    const ssid = document.getElementById('wifiSsid').value.trim();
    const password = document.getElementById('wifiPassword').value;

    if (!ssid) {
        showWiFiConfigMessage('error', '❌ SSID is required');
        return;
    }

    showWiFiConfigMessage('info', '⏳ Saving configuration and connecting...');

    try {
        const response = await fetch('/api/wifi/config', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ ssid, password })
        });

        if (!response.ok) {
            throw new Error('Failed to save configuration');
        }

        const result = await response.json();

        if (result.success) {
            showWiFiConfigMessage('success',
                '✅ Configuration saved! Connecting to WiFi...\n' +
                '⏳ Please wait 30 seconds then check if connected.\n' +
                'If successful, reconnect to your WiFi and access the new IP address.');

            // Clear password field
            document.getElementById('wifiPassword').value = '';

            // Reload status after delay
            setTimeout(() => {
                loadSystemStatus();
                showWiFiConfigMessage('info', '🔄 Checking connection status...');
            }, 10000);

            setTimeout(() => {
                loadSystemStatus();
            }, 30000);
        } else {
            showWiFiConfigMessage('error', '❌ Failed to save configuration');
        }
    } catch (error) {
        console.error('WiFi config error:', error);
        showWiFiConfigMessage('error', '❌ Error: ' + error.message);
    }
}

function showWiFiConfigMessage(type, message) {
    const messageDiv = document.getElementById('wifiConfigMessage');
    if (!messageDiv) {
        // Create message div if doesn't exist
        const form = document.getElementById('wifiForm');
        if (!form) return;

        const div = document.createElement('div');
        div.id = 'wifiConfigMessage';
        div.style.marginTop = '1rem';
        form.parentNode.insertBefore(div, form.nextSibling);
    }

    const msg = document.getElementById('wifiConfigMessage');
    msg.innerHTML = message.replace(/\n/g, '<br>');

    // Style based on type
    if (type === 'error') {
        msg.style.cssText = 'padding: 1rem; background: rgba(239,68,68,0.1); border: 1px solid #ef4444; border-radius: 0.5rem; color: #ef4444; margin-top: 1rem; white-space: pre-wrap;';
    } else if (type === 'success') {
        msg.style.cssText = 'padding: 1rem; background: rgba(16,185,129,0.1); border: 1px solid #10b981; border-radius: 0.5rem; color: #10b981; margin-top: 1rem; white-space: pre-wrap;';
    } else {
        msg.style.cssText = 'padding: 1rem; background: rgba(59,130,246,0.1); border: 1px solid #3b82f6; border-radius: 0.5rem; color: #3b82f6; margin-top: 1rem; white-space: pre-wrap;';
    }
}

// Scan for available WiFi networks
async function scanWiFiNetworks() {
    const btn = document.getElementById('scanWiFiBtn');
    const container = document.getElementById('wifiNetworksList');

    if (!container) return;

    if (btn) {
        btn.disabled = true;
        btn.innerHTML = '<span class="btn-icon">🔄</span> Scanning...';
    }

    container.innerHTML = '<div class="loading-spinner"><div class="spinner"></div><p>Scanning for networks...</p></div>';

    try {
        const response = await fetch('/api/wifi/scan', { method: 'POST' });

        if (!response.ok) {
            throw new Error('Scan failed');
        }

        const data = await response.json();

        if (data.networks && data.networks.length > 0) {
            displayWiFiNetworks(data.networks);
        } else {
            container.innerHTML = '<p class="empty-state">No networks found. Try scanning again.</p>';
        }
    } catch (error) {
        console.error('WiFi scan error:', error);
        container.innerHTML = '<p class="empty-state error">Scan failed. Please try again.</p>';
    } finally {
        if (btn) {
            btn.disabled = false;
            btn.innerHTML = '<span class="btn-icon">🔍</span> Scan Networks';
        }
    }
}

// Display scanned WiFi networks
function displayWiFiNetworks(networks) {
    const container = document.getElementById('wifiNetworksList');
    if (!container) return;

    // Sort by signal strength
    networks.sort((a, b) => b.rssi - a.rssi);

    let html = '<div class="wifi-networks-grid">';

    networks.forEach(network => {
        const signalStrength = getSignalStrength(network.rssi);
        const securityIcon = network.encryption !== 0 ? '🔒' : '🔓';
        const signalBars = getSignalBars(network.rssi);

        html += `
            <div class="wifi-network-item" onclick="selectWiFiNetwork('${escapeHtml(network.ssid)}')">
                <div class="wifi-network-info">
                    <div class="wifi-network-name">${securityIcon} ${escapeHtml(network.ssid)}</div>
                    <div class="wifi-network-details">
                        <span class="signal-indicator">${signalBars}</span>
                        <span class="signal-strength">${signalStrength}</span>
                        <span class="rssi-value">${network.rssi} dBm</span>
                    </div>
                </div>
                <div class="wifi-network-action">
                    <button type="button" class="btn-icon-only">→</button>
                </div>
            </div>
        `;
    });

    html += '</div>';
    container.innerHTML = html;
}

// Helper functions
function getSignalStrength(rssi) {
    if (rssi >= -50) return 'Excellent';
    if (rssi >= -60) return 'Good';
    if (rssi >= -70) return 'Fair';
    return 'Weak';
}

function getSignalBars(rssi) {
    if (rssi >= -50) return '📶📶📶📶';
    if (rssi >= -60) return '📶📶📶';
    if (rssi >= -70) return '📶📶';
    return '📶';
}

function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// Select a WiFi network from scan results
function selectWiFiNetwork(ssid) {
    const ssidInput = document.getElementById('wifiSsid');
    const passwordInput = document.getElementById('wifiPassword');

    if (ssidInput) {
        ssidInput.value = ssid;
        ssidInput.focus();
    }

    if (passwordInput) {
        passwordInput.focus();
    }

    // Scroll to form
    const form = document.getElementById('wifiForm');
    if (form) {
        form.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
    }
}

// Initialize WiFi settings when page loads
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initWiFiSettings);
} else {
    initWiFiSettings();
}

function initWiFiSettings() {
    // Load config when settings tab becomes active
    const settingsTab = document.getElementById('settings');
    if (settingsTab) {
        const observer = new MutationObserver((mutations) => {
            mutations.forEach((mutation) => {
                if (mutation.target.classList.contains('active')) {
                    loadWiFiConfig();
                }
            });
        });

        observer.observe(settingsTab, { attributes: true, attributeFilter: ['class'] });
    }
}
