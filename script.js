const patients = [];
const doctors = [];

const patientForm = document.querySelector("#patientForm");
const doctorForm = document.querySelector("#doctorForm");
const patientQueue = document.querySelector("#patientQueue");
const doctorList = document.querySelector("#doctorList");
const emptyQueue = document.querySelector("#emptyQueue");
const emptyDoctors = document.querySelector("#emptyDoctors");
const severityInput = document.querySelector("#patientSeverity");
const severityValue = document.querySelector("#severityValue");
const severityText = document.querySelector("#severityText");
const doctorSearch = document.querySelector("#doctorSearch");
const treatNextBtn = document.querySelector("#treatNextBtn");
const toast = document.querySelector("#toast");

function createId() {
  if (globalThis.crypto && globalThis.crypto.randomUUID) {
    return globalThis.crypto.randomUUID();
  }

  return `${Date.now()}-${Math.random().toString(16).slice(2)}`;
}

function severityMeta(severity) {
  if (severity >= 8) {
    return { label: "Critical", color: "#ff5d73" };
  }

  if (severity >= 5) {
    return { label: "Urgent", color: "#ffd166" };
  }

  return { label: "Stable", color: "#43e89d" };
}

function showToast(message) {
  toast.textContent = message;
  toast.classList.add("show");
  clearTimeout(showToast.timer);
  showToast.timer = setTimeout(() => toast.classList.remove("show"), 2400);
}

function sortPatients() {
  patients.sort((a, b) => b.severity - a.severity || a.createdAt - b.createdAt);
}

function updateStats() {
  const criticalCount = patients.filter((patient) => patient.severity >= 8).length;
  const nextPatient = patients[0];

  document.querySelector("#patientCount").textContent = patients.length;
  document.querySelector("#criticalCount").textContent = criticalCount;
  document.querySelector("#doctorCount").textContent = doctors.length;
  document.querySelector("#queueHealth").textContent = patients.length === 0 ? "Clear" : criticalCount > 0 ? "Critical" : "Active";
  document.querySelector("#nextPatientName").textContent = nextPatient ? nextPatient.name : "No patient waiting";
}

function renderPatients() {
  sortPatients();
  patientQueue.innerHTML = "";
  emptyQueue.classList.toggle("visible", patients.length === 0);

  patients.forEach((patient, index) => {
    const meta = severityMeta(patient.severity);
    const card = document.createElement("article");
    card.className = "patient-card";
    card.style.setProperty("--severity-color", meta.color);
    card.style.setProperty("--severity-width", `${patient.severity * 10}%`);

    card.innerHTML = `
      <div class="card-top">
        <div>
          <strong>${index + 1}. ${patient.name}</strong>
          <small>Age ${patient.age} &middot; Added ${patient.time}</small>
        </div>
        <span class="severity-pill">${meta.label} ${patient.severity}/10</span>
      </div>
      <div class="severity-track"><div class="severity-fill"></div></div>
      <div class="card-actions">
        <button class="mini-btn" data-action="boost" data-id="${patient.id}">+ Priority</button>
        <button class="mini-btn danger-btn" data-action="remove" data-id="${patient.id}">Remove</button>
      </div>
    `;

    patientQueue.appendChild(card);
  });

  updateStats();
}

function renderDoctors() {
  const query = doctorSearch.value.trim().toLowerCase();
  const visibleDoctors = doctors.filter((doctor) => {
    return doctor.name.toLowerCase().includes(query) || String(doctor.id).includes(query);
  });

  doctorList.innerHTML = "";
  emptyDoctors.classList.toggle("visible", visibleDoctors.length === 0);

  visibleDoctors.forEach((doctor) => {
    const card = document.createElement("article");
    card.className = "doctor-card";
    card.innerHTML = `
      <div>
        <strong>${doctor.name}</strong>
        <small>${doctor.specialization}</small>
      </div>
      <span class="doctor-id">#${doctor.id}</span>
    `;
    doctorList.appendChild(card);
  });

  updateStats();
}

severityInput.addEventListener("input", () => {
  const severity = Number(severityInput.value);
  const meta = severityMeta(severity);
  severityValue.textContent = severity;
  severityText.textContent = meta.label;
  severityText.style.color = meta.color;
});

patientForm.addEventListener("submit", (event) => {
  event.preventDefault();

  const name = document.querySelector("#patientName").value.trim();
  const age = Number(document.querySelector("#patientAge").value);
  const severity = Number(severityInput.value);

  patients.push({
    id: createId(),
    name,
    age,
    severity,
    createdAt: Date.now(),
    time: new Date().toLocaleTimeString([], { hour: "2-digit", minute: "2-digit" })
  });

  patientForm.reset();
  severityInput.value = 5;
  severityInput.dispatchEvent(new Event("input"));
  renderPatients();
  showToast(`${name} added to the priority queue.`);
});

doctorForm.addEventListener("submit", (event) => {
  event.preventDefault();

  const name = document.querySelector("#doctorName").value.trim();
  const id = Number(document.querySelector("#doctorId").value);
  const specialization = document.querySelector("#doctorSpecialization").value.trim();

  if (doctors.some((doctor) => doctor.id === id)) {
    showToast(`Doctor ID ${id} already exists.`);
    return;
  }

  doctors.push({ name, id, specialization });
  doctorForm.reset();
  renderDoctors();
  showToast(`${name} added to the care team.`);
});

patientQueue.addEventListener("click", (event) => {
  const button = event.target.closest("button");
  if (!button) return;

  const patient = patients.find((item) => item.id === button.dataset.id);
  if (!patient) return;

  if (button.dataset.action === "boost") {
    patient.severity = Math.min(10, patient.severity + 1);
    showToast(`${patient.name}'s priority increased.`);
  }

  if (button.dataset.action === "remove") {
    patients.splice(patients.indexOf(patient), 1);
    showToast(`${patient.name} removed from the queue.`);
  }

  renderPatients();
});

treatNextBtn.addEventListener("click", () => {
  sortPatients();
  const nextPatient = patients.shift();

  if (!nextPatient) {
    showToast("No patients are waiting right now.");
    return;
  }

  renderPatients();
  showToast(`${nextPatient.name} moved to treatment.`);
});

doctorSearch.addEventListener("input", renderDoctors);

patients.push(
  { id: createId(), name: "Maya Hassan", age: 42, severity: 9, createdAt: Date.now() - 3000, time: "09:10 AM" },
  { id: createId(), name: "Yousef Karim", age: 27, severity: 6, createdAt: Date.now() - 2000, time: "09:12 AM" },
  { id: createId(), name: "Lina Samir", age: 31, severity: 3, createdAt: Date.now() - 1000, time: "09:15 AM" }
);

doctors.push(
  { name: "Dr. Omar Ali", id: 101, specialization: "Emergency Medicine" },
  { name: "Dr. Nour Salem", id: 204, specialization: "Cardiology" }
);

severityInput.dispatchEvent(new Event("input"));
renderPatients();
renderDoctors();
