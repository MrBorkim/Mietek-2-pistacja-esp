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
        msg.style.cssText = 'padding: 1rem; background: rgba(239,68,68,0.1); border: 1px solid #ef4444; border-radius: 0.5rem; color: #ef4444; margin-top: 1rem;';
    } else if (type === 'success') {
        msg.style.cssText = 'padding: 1rem; background: rgba(16,185,129,0.1); border: 1px solid #10b981; border-radius: 0.5rem; color: #10b981; margin-top: 1rem;';
    } else {
        msg.style.cssText = 'padding: 1rem; background: rgba(59,130,246,0.1); border: 1px solid #3b82f6; border-radius: 0.5rem; color: #3b82f6; margin-top: 1rem;';
    }
}

// Auto-load WiFi config when Settings tab is opened
function switchTab(tabName) {
    // Update navigation buttons
    document.querySelectorAll('.nav-btn').forEach(btn => {
        btn.classList.toggle('active', btn.dataset.tab === tabName);
    });

    // Update content sections
    document.querySelectorAll('.tab-content').forEach(content => {
        content.classList.toggle('active', content.id === tabName);
    });

    // Load WiFi config when opening settings
    if (tabName === 'settings') {
        loadWiFiConfig();
        loadSystemStatus();
    }
}
